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

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from RLECompression generation")
#endif

	auto* CurrentRun = &IndexParams.VoxelGrid->GetData()[0]; 
	// Set first run to trigger first condition in while loop
	IndexParams.CurrentInterval.NextIntervalEnds[0] = {CurrentRun, false, false, 0, 0};
	IndexParams.CurrentInterval.NextIntervalEnds[0].RunEnd = CurrentRun->RunLenght;
	
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	
	const int XIndex = VoxelGenerator->CalculateVoxelIndex(1,0,0);
	const int ZIndex = VoxelGenerator->CalculateVoxelIndex(0,0,1);
	
	IndexParams.CurrentInterval.NextIntervalEnds[1] = { CurrentRun, true, true, ChunkDimension * ChunkDimension};
	
	// Traverse through voxel grid
	for (int x = 0; x < ChunkDimension; x++)
	{
		IndexParams.CurrentInterval.NextIntervalEnds[2] = { IndexParams.CurrentInterval.NextIntervalEnds[0].CurrentRun, true, true, IndexParams.CurrentInterval.TraversedVoxelSequence +  ChunkDimension};
		
		for (int z = 0; z < ChunkDimension; z++)
		{
			IndexParams.CurrentInterval.Y = 0;
			
			do
			{
				// Calculate index
				// Smallest interval should always be increase of Y dimension
				int MinValue = IndexParams.CurrentInterval.TraversedVoxelSequence +  ChunkDimension;
				FRLEVoxel* CurrentVoxel = IndexParams.CurrentInterval.NextIntervalEnds[0].CurrentRun;

				int IntervalFlag = 0;
				
				for (int32 i = 0; i < 3; i++)
				{
					auto& Interval = IndexParams.CurrentInterval.NextIntervalEnds[i];

					int EmptyVoxel = ((Interval.IsOuterInterval && Interval.ShowBorders)  || Interval.CurrentRun->IsVoxelEmpty());
					IntervalFlag |= EmptyVoxel << i;

					if (Interval.RunEnd < MinValue)
					{
						MinValue = Interval.RunEnd;
						CurrentVoxel = Interval.CurrentRun;
					}
				}

				IndexParams.CurrentInterval.CurrentIntervalType = static_cast<EIntervalType>(IntervalFlag);
				
				IndexParams.CurrentInterval.IntervalEnd = MinValue - IndexParams.CurrentInterval.TraversedVoxelSequence;
				
				// Generate run faces
				auto InitialPosition = FIntVector(x, IndexParams.CurrentInterval.Y, z);
				
				if (IndexParams.CurrentInterval.CurrentIntervalType != EmptyFace && IndexParams.CurrentInterval.CurrentIntervalType != FullCulledFace)
				{
					// Front
					if (IndexParams.CurrentInterval.CurrentIntervalType == FrontFace || IndexParams.CurrentInterval.CurrentIntervalType == FrontTopFace)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::FrontFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);
					}

					// Back
					if (IndexParams.CurrentInterval.CurrentIntervalType == BackFace || IndexParams.CurrentInterval.CurrentIntervalType == BackBottomFace)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::BackFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);
					}

					// Top
					if (IndexParams.CurrentInterval.CurrentIntervalType == TopFace || IndexParams.CurrentInterval.CurrentIntervalType == FrontTopFace)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::TopFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);
					}

					// Bottom
					if (IndexParams.CurrentInterval.CurrentIntervalType == BottomFace || IndexParams.CurrentInterval.CurrentIntervalType == BackBottomFace)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::BottomFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);
					}
				}

				// Right
				CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);

				// Left
				CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, *CurrentVoxel, IndexParams.CurrentInterval.IntervalEnd);

				IndexParams.CurrentInterval.TraversedVoxelSequence += IndexParams.CurrentInterval.IntervalEnd;
				IndexParams.CurrentInterval.Y += IndexParams.CurrentInterval.IntervalEnd;

				for (int32 i = 0; i < 3; i++)
				{
					if (IndexParams.CurrentInterval.NextIntervalEnds[i].RunEnd == IndexParams.CurrentInterval.TraversedVoxelSequence)
					{
						auto& Interval = IndexParams.CurrentInterval.NextIntervalEnds[i];
						Interval.RunIndex++;
						Interval.CurrentRun = &IndexParams.VoxelGrid->GetData()[Interval.RunIndex];
						Interval.RunEnd = Interval.CurrentRun->RunLenght + IndexParams.CurrentInterval.TraversedVoxelSequence;
						Interval.IsOuterInterval = false;
						Interval.ShowBorders = false;
					}
				}
				
			} while (IndexParams.CurrentInterval.Y < ChunkDimension);
		}
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

	PreallocateArrays(MeshVars);

	
	TMap<uint32, uint32> LocalVoxelTable;

	FaceGeneration(IndexParams, MeshVars, LocalVoxelTable);
	
	GenerateProcMesh(MeshVars, LocalVoxelTable);
	
	if (IndexParams.VoxelChange != nullptr)
	{
		VoxelGridPtr->RLEVoxelGrid = IndexParams.NewVoxelGrid;
	}
}

void URLERunDirectionalVoxelMesher::CreateFace(const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable, const FStaticMergeData& StaticData,
                                          const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel, const int YEnd) const
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	const auto FaceIndex = static_cast<uint8>(StaticData.FaceDirection);
	
	ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, FaceIndex);
}

