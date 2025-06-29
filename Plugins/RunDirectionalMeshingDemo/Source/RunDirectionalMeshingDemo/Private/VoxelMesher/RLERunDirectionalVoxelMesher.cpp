#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

#include "Log/VoxelMeshingProfilingLogger.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid)
{

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif
	
	auto VoxelGridObject = NewObject<URLEVoxelGrid>();

	const auto RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

	RLEVoxelGrid->Reserve(Chunk.ChunkVoxelIdTable.Num());
	RLEVoxelGrid->Emplace(1, VoxelGrid[0]);

	for (int32 x = 1; x < VoxelGrid.Num(); x++)
	{
		const FVoxel Voxel = VoxelGrid[x];
		if (RLEVoxelGrid->Last().Voxel == Voxel)
		{
			RLEVoxelGrid->Last().RunLenght++;
		}
		else
		{
			RLEVoxelGrid->Emplace(1, Voxel);
		}
	}

	VoxelGridObject->RLEVoxelGrid = RLEVoxelGrid;
	Chunk.VoxelModel = VoxelGridObject;

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	const FString MapName = GetWorld()->GetMapName();
	FVoxelMeshingProfilingLogger::LogAllocatedMemory(MapName, VoxelGridObject->RLEVoxelGrid->GetAllocatedSize());
#endif
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from RLECompression generation")
#endif

	bool bEdited = false;

	IndexParams.CurrentRLERun = IndexParams.VoxelGrid->GetData()[0];
	IndexParams.TraversedRun = IndexParams.CurrentRLERun.RunLenght;

	int YEnd;

	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();

	const int XIndex = VoxelGenerator->CalculateVoxelIndex(1,0,0);
	const int ZIndex = VoxelGenerator->CalculateVoxelIndex(0,0,1);

	// Traverse through voxel grid
	for (int x = 0; x < ChunkDimension; x++)
	{
		for (int z = 0; z < ChunkDimension; z++)
		{
			IndexParams.YStart = 0;
			bool bIsNewRun;

			// Calculate index
			while (IndexParams.YStart < ChunkDimension)
			{
				if (IndexParams.TraversedRun == IndexParams.CurrentRLERun.RunLenght)
				{
					if (IndexParams.EditAreaIndex == 0)
					{
						bIsNewRun = true;
						IndexParams.RunIndex++;
						IndexParams.CurrentRLERun = IndexParams.VoxelGrid->GetData()[IndexParams.RunIndex];
					}
					else
					{
						bIsNewRun = false;
						IndexParams.CurrentRLERun = IndexParams.NewVoxelGrid->GetData()[IndexParams.NewVoxelGrid->Num()
							- IndexParams.EditAreaIndex];
						IndexParams.EditAreaIndex--;
					}

					YEnd = IndexParams.CurrentRLERun.RunLenght;
					IndexParams.TraversedRun = 0;
				}
				else
				{
					bIsNewRun = false;
					YEnd = IndexParams.CurrentRLERun.RunLenght - IndexParams.TraversedRun;
				}

				// No action is need when in edit area, otherwise try to apply change and add next run to a new array
				if (IndexParams.VoxelChange != nullptr)
				{
					// Add only full runs
					if (bIsNewRun)
					{
						// Add only outside edit area
						IndexParams.NewVoxelGrid->Push(IndexParams.CurrentRLERun);
					}

					//MID RUN EDIT
					if (!bEdited && x == IndexParams.VoxelChange->VoxelPosition.X && z == IndexParams.VoxelChange->VoxelPosition.Z &&
						IndexParams.YStart <= IndexParams.VoxelChange->VoxelPosition.Y)
					{
						const auto RunEnd = IndexParams.YStart + YEnd;
						if (RunEnd >= IndexParams.VoxelChange->VoxelPosition.Y)
						{
							// Calculated once per meshing
							bEdited = true;

							if (!CalculateStartRunEditIndex(IndexParams, RunEnd))
							{
								return;
							}

							continue;
						}
					}
				}
				
				// Step to end
				if (IndexParams.YStart + YEnd > ChunkDimension)
				{
					YEnd = ChunkDimension - IndexParams.YStart;
				}
				
				if (!IndexParams.CurrentRLERun.IsVoxelEmpty())
				{
					// Generate run faces
					auto InitialPosition = FIntVector(x, IndexParams.YStart, z);

					// Tail culling
					if (IndexParams.TraversedRun - XIndex < 0)
					{
						// Back
						CreateFace(MeshVars, FStaticMergeData::BackFaceData, InitialPosition, IndexParams.CurrentRLERun,
							   YEnd);
					}

					// Front
					CreateFace(MeshVars, FStaticMergeData::FrontFaceData, InitialPosition, IndexParams.CurrentRLERun, YEnd);

					// Top
					CreateFace(MeshVars, FStaticMergeData::TopFaceData, InitialPosition, IndexParams.CurrentRLERun, YEnd);
						
					// Tail culling
					if (z == 0 || IndexParams.TraversedRun - ZIndex < 0){
						// Bottom
						CreateFace(MeshVars, FStaticMergeData::BottomFaceData, InitialPosition, IndexParams.CurrentRLERun,
					           YEnd);
					}
					
					// Right
					CreateFace(MeshVars, FStaticMergeData::RightFaceData, InitialPosition, IndexParams.CurrentRLERun,
					           YEnd);

					// Left
					CreateFace(MeshVars, FStaticMergeData::LeftFaceData, InitialPosition, IndexParams.CurrentRLERun,
					           YEnd);
				}
				
				IndexParams.TraversedRun += YEnd;
				IndexParams.YStart += YEnd;
			}

			// END RUN EDIT
			if (IndexParams.VoxelChange != nullptr && !bEdited && IndexParams.VoxelChange->VoxelPosition.Y == 0 && (x == IndexParams.VoxelChange->
				VoxelPosition.X && z + 1 ==
				IndexParams.VoxelChange->VoxelPosition.Z || x + 1 == IndexParams.VoxelChange->VoxelPosition.X && IndexParams.VoxelChange->VoxelPosition.Z ==
				0 && z == ChunkDimension - 1))
			{
				// Calculated once per meshing
				bEdited = true;

				if (!CalculateBorderRunEditIndex(IndexParams))
				{
					return;
				}
			}
		}
	}

	if (IndexParams.VoxelChange != nullptr)
	{
		if (!IndexParams.EditVoxel.IsEmptyVoxel())
		{
			MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable[IndexParams.EditVoxel.VoxelId]++;
		}

		if (!IndexParams.ReplacedVoxel.IsEmptyVoxel())
		{
			MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable[IndexParams.ReplacedVoxel.VoxelId]--;

			if (MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable[IndexParams.ReplacedVoxel.VoxelId] <= 0)
			{
				MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.Remove(IndexParams.ReplacedVoxel.VoxelId);
			}
		}

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
		const FString MapName = GetWorld()->GetMapName();
		FVoxelMeshingProfilingLogger::LogAllocatedMemory(MapName, IndexParams.NewVoxelGrid->GetAllocatedSize());
#endif
	}
}

void URLERunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, FVoxelChange* VoxelChange)
{
	if (EmptyActor(MeshVars))
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<URLEVoxelGrid>(MeshVars.ChunkParams.OriginalChunk->VoxelModel);

	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	FIndexParams IndexParams;
	IndexParams.VoxelGrid = VoxelGridPtr->RLEVoxelGrid;
	
	if (VoxelChange != nullptr)
	{
		IndexParams.VoxelChange = VoxelChange;
		IndexParams.NewVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		IndexParams.NewVoxelGrid->Reserve(IndexParams.VoxelGrid->Num() + 1);
		IndexParams.EditVoxel = VoxelGenerator->GetVoxelByName(VoxelChange->VoxelName);

		if (!IndexParams.EditVoxel.IsEmptyVoxel())
		{
			auto& VoxelTable = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable;
			if (!VoxelTable.Contains(IndexParams.EditVoxel.VoxelId))
			{
				VoxelTable.Add(IndexParams.EditVoxel.VoxelId, 0);
			}
		}
	}
	else
	{
		IndexParams.NewVoxelGrid = nullptr;
	}

	InitFaceContainers(MeshVars);

	FaceGeneration(IndexParams, MeshVars);
	
	GenerateMeshFromFaces(MeshVars);
	
	if (IndexParams.VoxelChange != nullptr)
	{
		VoxelGridPtr->RLEVoxelGrid = IndexParams.NewVoxelGrid;
	}
}

void URLERunDirectionalVoxelMesher::CreateFace(FMesherVariables& MeshVars, const FStaticMergeData& StaticData,
                                          const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel, const int YEnd)
{
	const int LocalVoxelId = MeshVars.VoxelIdToLocalVoxelMap[RLEVoxel.Voxel.VoxelId];
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	const auto FaceContainerIndex = static_cast<uint8>(StaticData.FaceSide);
	MeshVars.Faces[FaceContainerIndex][LocalVoxelId]->Push(NewFace);
}

bool URLERunDirectionalVoxelMesher::CalculateStartRunEditIndex(FIndexParams& IndexParams, const int RunEnd)
{
	if (RunEnd == IndexParams.VoxelChange->VoxelPosition.Y)
	{
		auto& LastRLERun = IndexParams.NewVoxelGrid->Last();

		if (LastRLERun.Voxel == IndexParams.EditVoxel)
		{
			LastRLERun.RunLenght++;
		}
		else
		{
			IndexParams.NewVoxelGrid->Emplace(1, IndexParams.EditVoxel);
			IndexParams.EditAreaIndex = 1;
		}

		const auto NextRunIndex = IndexParams.RunIndex + 1;

		if (IndexParams.VoxelGrid->IsValidIndex(NextRunIndex))
		{
			auto& NextRLERun = IndexParams.VoxelGrid->GetData()[NextRunIndex];

			if (NextRLERun.Voxel == IndexParams.EditVoxel)
			{
				return false;
			}

			FirstRunEditIndex(IndexParams);
		}

		IndexParams.CurrentRLERun = LastRLERun;
	}
	else if (IndexParams.CurrentRLERun.Voxel != IndexParams.EditVoxel)
	{
		//This code is activated only for first voxel in voxel model at position 0, 0, 0
		if (IndexParams.VoxelChange->VoxelPosition.Y == 0)
		{
			auto& LastRLERun = IndexParams.NewVoxelGrid->Last();
			auto TempRun = LastRLERun;
			IndexParams.ReplacedVoxel = TempRun.Voxel;
			LastRLERun.RunLenght = 1;
			LastRLERun.Voxel = IndexParams.EditVoxel;
			TempRun.RunLenght--;

			if (TempRun.IsRunEmpty())
			{
				auto NextRLERun = IndexParams.VoxelGrid->GetData()[IndexParams.RunIndex + 1];
				if (NextRLERun.Voxel == IndexParams.EditVoxel)
				{
					IndexParams.RunIndex++;
					LastRLERun.RunLenght += NextRLERun.RunLenght;
				}
			}
			else
			{
				IndexParams.NewVoxelGrid->Push(TempRun);
				IndexParams.EditAreaIndex = 1;
			}

			IndexParams.CurrentRLERun = LastRLERun;
		}
		else
		{
			const auto MidRunLenght = IndexParams.TraversedRun + IndexParams.VoxelChange->VoxelPosition.Y - IndexParams.
				YStart;
			const int EndRunLength = IndexParams.NewVoxelGrid->Last().RunLenght - MidRunLenght - 1;

			CalculateMidRun(MidRunLenght, EndRunLength, IndexParams);
		}
	}
	else
	{
		return false;
	}

	return true;
}

bool URLERunDirectionalVoxelMesher::CalculateBorderRunEditIndex(FIndexParams& IndexParams)
{
	auto& LastRLERun = IndexParams.NewVoxelGrid->Last();

	if (IndexParams.TraversedRun != LastRLERun.RunLenght)
	{
		if (LastRLERun.Voxel == IndexParams.EditVoxel)
		{
			return false;
		}

		CalculateMidRun(IndexParams.TraversedRun, LastRLERun.RunLenght - IndexParams.TraversedRun - 1, IndexParams);
	}
	else
	{
		if (LastRLERun.Voxel == IndexParams.EditVoxel)
		{
			LastRLERun.RunLenght++;
			FirstRunEditIndex(IndexParams);
		}
		else if (IndexParams.VoxelGrid->IsValidIndex(IndexParams.RunIndex + 1))
		{
			const auto& NextRLERun = IndexParams.VoxelGrid->GetData()[IndexParams.RunIndex + 1];

			if (NextRLERun.Voxel == IndexParams.EditVoxel)
			{
				return false;
			}

			IndexParams.EditAreaIndex = 1;
			IndexParams.NewVoxelGrid->Emplace(1, IndexParams.EditVoxel);

			FirstRunEditIndex(IndexParams);
		}

		IndexParams.CurrentRLERun = LastRLERun;
	}

	return true;
}

void URLERunDirectionalVoxelMesher::CalculateMidRun(const int MidRunLenght, const int EndRunLength,
                                                 FIndexParams& IndexParams)
{
	auto& LastRLERun = IndexParams.NewVoxelGrid->Last();
	const FRLEVoxel SplitRLERun(EndRunLength, LastRLERun.Voxel);
	LastRLERun.RunLenght = MidRunLenght;
	IndexParams.ReplacedVoxel = LastRLERun.Voxel;
	IndexParams.CurrentRLERun = LastRLERun;

	if (SplitRLERun.IsRunEmpty())
	{
		const int NextIndex = IndexParams.RunIndex + 1;
		if (IndexParams.VoxelGrid->IsValidIndex(NextIndex) && IndexParams.VoxelGrid->GetData()[NextIndex].Voxel ==
			IndexParams.EditVoxel)
		{
			IndexParams.VoxelGrid->GetData()[NextIndex].RunLenght++;
		}
		else
		{
			IndexParams.NewVoxelGrid->Emplace(1, IndexParams.EditVoxel);
			IndexParams.EditAreaIndex = 1;
		}
	}
	else
	{
		IndexParams.NewVoxelGrid->Emplace(1, IndexParams.EditVoxel);
		IndexParams.NewVoxelGrid->Push(SplitRLERun);
		IndexParams.EditAreaIndex = 2;
	}
}

void URLERunDirectionalVoxelMesher::FirstRunEditIndex(FIndexParams& IndexParams)
{
	auto& NextRLERun = IndexParams.VoxelGrid->GetData()[IndexParams.RunIndex + 1];
	auto& LastRLERun = IndexParams.NewVoxelGrid->Last();

	IndexParams.ReplacedVoxel = NextRLERun.Voxel;
	NextRLERun.RunLenght--;

	if (NextRLERun.IsRunEmpty())
	{
		IndexParams.RunIndex++;
		const auto NextNextIndex = IndexParams.RunIndex + 1;
		if (IndexParams.VoxelGrid->IsValidIndex(NextNextIndex))
		{
			const auto& NextNextRLERun = IndexParams.VoxelGrid->GetData()[NextNextIndex];
			if(NextNextRLERun.Voxel == LastRLERun.Voxel)
			{
				IndexParams.RunIndex++;
				LastRLERun.RunLenght += NextNextRLERun.RunLenght;
			}
		}
	}
}
