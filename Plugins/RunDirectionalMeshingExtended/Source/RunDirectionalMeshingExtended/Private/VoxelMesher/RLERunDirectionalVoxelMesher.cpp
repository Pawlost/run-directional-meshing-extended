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

URLERunDirectionalVoxelMesher::URLERunDirectionalVoxelMesher()
{
	// FullCulledFace
	IntervalFaces[EIntervalType::FullCulledFaceInterval] = {};

	// FrontTopFace
	IntervalFaces[EIntervalType::FrontTopFaceInterval] = {
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX},
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BackFace
	IntervalFaces[EIntervalType::BackFaceInterval] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// TopFace
	IntervalFaces[EIntervalType::TopFaceInterval] = {
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BottomFace
	IntervalFaces[EIntervalType::BottomFaceInterval] = {
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// FrontFace
	IntervalFaces[EIntervalType::FrontFaceInterval] = {
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX}
	};

	// BackBottomFace
	IntervalFaces[EIntervalType::BackBottomFaceInterval] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading},
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// EmptyFace
	IntervalFaces[EIntervalType::EmptyFaceInterval] = {};
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars,
                                                   TMap<uint32, uint32>& LocalVoxelTable)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif
	
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const int MaxChunkVoxelSequence = VoxelGenerator->GetVoxelCountPerChunk();
	const int SequenceInLayer = VoxelGenerator->GetVoxelCountPerChunkLayer();

	const auto& GridData = *IndexParams.VoxelGrid;
	const auto FirstRun = &GridData[0];
	// Set first run to trigger first condition in while loop
	

	//TODO: move set num to preallocation
	for (int32 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		MeshVars.BorderChunks[f].SetNum(SequenceInLayer);
	}
	
	IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading] = {FirstRun, FirstRun->RunLenght, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingX] = {FirstRun,  FirstRun->RunLenght + SequenceInLayer, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingZ] = {FirstRun, FirstRun->RunLenght + ChunkDimension, 0 };
	
	int IntervalEnd = 0;
	FVoxelFace PreviousFaces[CHUNK_FACE_COUNT];

	auto InitialPosition = FIntVector(0, 0, 0);

	const FRLEVoxel* PreviousRun = nullptr;
	
	// Traverse through voxel grid
	while (IndexParams.TraversedVoxelSequence < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.TraversedVoxelSequence / (SequenceInLayer);
		const int Z = ((IndexParams.TraversedVoxelSequence / ChunkDimension) % ChunkDimension);
		int Y = IndexParams.TraversedVoxelSequence % ChunkDimension;
		
		// Check borders
		do
		{
			// Reset interval flag
			IndexParams.IntervalFlag = 0;
			
			if (AdvanceInterval(IndexParams, EIntervalEndIndex::Leading, 0, 0))
			{
				auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];

				// Generate Left and Right faces when interval advances
				if (PreviousRun->Voxel.IsEmptyVoxel())
				{
					InitialPosition = FIntVector(X, Y, Z);
					
					// Left
					if (Y != 0)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, *LeadingInterval.CurrentRun, IntervalEnd, PreviousFaces);
					}else
					{
						CreateLeftFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);		
					}
				}
				else if (LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel())
				{
					// Right
					if(InitialPosition.Y + IntervalEnd != ChunkDimension)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition, *PreviousRun, IntervalEnd, PreviousFaces);
					}
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension
			auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];

			if (X == 0)
			{
				CreateBackFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);
			}

			if (X == ChunkDimension - 1)
			{
				CreateFrontFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);
			}
			
			if (Y == 0)
			{
				CreateLeftFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);		
			}
			
            if (Z == ChunkDimension - 1)
            {
				CreateTopFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);		
            }

			if (Z == 0)
			{
				CreateBottomFace(MeshVars, X, Y, Z, *LeadingInterval.CurrentRun);		
			}
			
			IndexParams.MinValue = LeadingInterval.RunEnd;

			AdvanceInterval(IndexParams, EIntervalEndIndex::FollowingX, X == 0, LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel());
			AdvanceInterval(IndexParams, EIntervalEndIndex::FollowingZ, Z == 0, LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel());
			
			// Generate culled faces
			if (IndexParams.IntervalFlag != EIntervalType::EmptyFaceInterval)
			{
				IndexParams.MinValue = FMath::Min(IndexParams.MinValue,(ChunkDimension - Y) + IndexParams.TraversedVoxelSequence);

				IntervalEnd = IndexParams.MinValue - IndexParams.TraversedVoxelSequence;

				InitialPosition = FIntVector(X, Y, Z);
				
				// Inner culling
				for (const auto& [FaceData, Offset, IntervalEndIndex] : IntervalFaces[IndexParams.IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition + Offset,
							   *IndexParams.NextIntervalEnds[IntervalEndIndex].CurrentRun, IntervalEnd, PreviousFaces);
				}
			}
			else
			{
				IntervalEnd = IndexParams.MinValue - IndexParams.TraversedVoxelSequence;
			}
			
			IndexParams.TraversedVoxelSequence = IndexParams.MinValue;
			Y += IntervalEnd;

			PreviousRun = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading].CurrentRun;

		} while (Y < ChunkDimension);

		if(Y  == ChunkDimension)
		{
			CreateRightFace(MeshVars, X, Y, Z, *PreviousRun);	
		}
	}

	// TODO: rewrite
	// TODO: move
	for (int direction = 0; direction < CHUNK_FACE_COUNT; direction++)
	{
		auto& PreviousFace = PreviousFaces[direction];
		if (!PreviousFace.Voxel.IsEmptyVoxel())
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, PreviousFace, LocalVoxelTable, direction);
		}
	}
		
	const FRLEVoxel* Voxel = nullptr;
	const FRLEVoxel* Voxel2 = nullptr;
	const FRLEVoxel* Voxel3 = nullptr;
	const FRLEVoxel* Voxel4 = nullptr;
	const FRLEVoxel* Voxel5 = nullptr;
	const FRLEVoxel* Voxel6 = nullptr;
		
	for (int x = 0; x < ChunkDimension; x++)
	{
		for (int y = 0; y < ChunkDimension; y++)
		{
			InitialPosition = FIntVector(0, y, x);
			auto Index = VoxelGenerator->CalculateVoxelIndex(InitialPosition);
			
			Voxel = &MeshVars.BorderChunks[EFaceDirection::Back][Index];
			if (Voxel->RunLenght > 1)
			{
				FRLEVoxel TempVoxel = *Voxel;
				TempVoxel.RunLenght--;
				if (MeshVars.BorderChunks[EFaceDirection::Back].IsValidIndex(Index + 1))
				{
					auto& NextIndex = MeshVars.BorderChunks[EFaceDirection::Back][Index + 1];
					if (NextIndex.RunLenght == 0)
					{
						NextIndex = TempVoxel;
					}
				}
			}

			Voxel4 = &MeshVars.BorderChunks[EFaceDirection::Front][Index];
			if (Voxel4->RunLenght > 1)
			{
				FRLEVoxel TempVoxel = *Voxel4;
				TempVoxel.RunLenght--;
				if (MeshVars.BorderChunks[EFaceDirection::Front].IsValidIndex(Index + 1))
				{
					auto& NextIndex = MeshVars.BorderChunks[EFaceDirection::Front][Index + 1];
					if (NextIndex.RunLenght == 0)
					{
						NextIndex = TempVoxel;
					}
				}
			}

			Voxel5 = &MeshVars.BorderChunks[EFaceDirection::Top][Index];
			if (Voxel5->RunLenght > 1)
			{
				FRLEVoxel TempVoxel = *Voxel5;
				TempVoxel.RunLenght--;
				if (MeshVars.BorderChunks[EFaceDirection::Top].IsValidIndex(Index + 1))
				{
					auto& NextIndex = MeshVars.BorderChunks[EFaceDirection::Top][Index + 1];
					if (NextIndex.RunLenght == 0)
					{
						NextIndex = TempVoxel;
					}
				}
			}

			Voxel6 = &MeshVars.BorderChunks[EFaceDirection::Bottom][Index];
			if (Voxel6->RunLenght > 1)
			{
				FRLEVoxel TempVoxel = *Voxel6;
				TempVoxel.RunLenght--;
				if (MeshVars.BorderChunks[EFaceDirection::Bottom].IsValidIndex(Index + 1))
				{
					auto& NextIndex = MeshVars.BorderChunks[EFaceDirection::Bottom][Index + 1];
					if (NextIndex.RunLenght == 0)
					{
						NextIndex = TempVoxel;
					}
				}
			}
			
			Voxel2 = &MeshVars.BorderChunks[EFaceDirection::Left][Index];
			Voxel3 = &MeshVars.BorderChunks[EFaceDirection::Right][Index];

			if (!Voxel->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(0, y, x);
				const FVoxelFace NewFace = BackFaceTemplate.StaticMeshingData.FaceCreator(Voxel->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Back);
			}

			if (!Voxel2->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(y, 0, x);
				const FVoxelFace NewFace = LeftFaceTemplate.StaticMeshingData.FaceCreator(Voxel2->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Left);
			}

			if (!Voxel3->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(y, ChunkDimension - 1, x);
				const FVoxelFace NewFace = RightFaceTemplate.StaticMeshingData.FaceCreator(Voxel3->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Right);
			}

			if (!Voxel4->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(ChunkDimension - 1, y, x);
				const FVoxelFace NewFace = FrontFaceTemplate.StaticMeshingData.FaceCreator(Voxel4->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Front);
			}

			if (!Voxel5->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(x, y, ChunkDimension - 1);
				const FVoxelFace NewFace = TopFaceTemplate.StaticMeshingData.FaceCreator(Voxel5->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Top);
			}

			if (!Voxel6->IsVoxelEmpty())
			{
				InitialPosition = FIntVector(x, y, 0);
				const FVoxelFace NewFace = BottomFaceTemplate.StaticMeshingData.FaceCreator(Voxel6->Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, EFaceDirection::Bottom);
			}
		}
	}
}

bool URLERunDirectionalVoxelMesher::AdvanceInterval(FIndexParams& IndexParams, const EIntervalEndIndex IntervalFlagIndex,
	const bool BorderCondition, const bool LeadingIntervalEmpty)
{
	bool AdvanceInterval = false;
	if (auto& Interval = IndexParams.NextIntervalEnds[IntervalFlagIndex]; Interval.RunEnd == IndexParams.TraversedVoxelSequence)
	{
		// Advance interval in chunk voxel sequence
		Interval.RunIndex++;
		Interval.CurrentRun = &(*IndexParams.VoxelGrid)[Interval.RunIndex];
		Interval.RunEnd = Interval.CurrentRun->RunLenght + IndexParams.TraversedVoxelSequence;
		AdvanceInterval = true;
	}

	// Sample interval
	
	// Move interval one step ahead if at the run end
	const auto& Interval = IndexParams.NextIntervalEnds[IntervalFlagIndex];
	const bool IntervalEmptyVoxel = Interval.CurrentRun->IsVoxelEmpty();

	// Create mask based on interval combination, the mask is used as an index to array to prevent branching
	// Indexes are described in github documentation
	// Set interval flag
	// Using xor at the chunk border prevents generating quads facing inside
	IndexParams.IntervalFlag |= ((BorderCondition && (LeadingIntervalEmpty != IntervalEmptyVoxel)) ^ IntervalEmptyVoxel) << IntervalFlagIndex;
	IndexParams.MinValue = FMath::Min(Interval.RunEnd, IndexParams.MinValue);

	return AdvanceInterval;
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

void URLERunDirectionalVoxelMesher::CreateFace(const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable,
                                               const FStaticMergeData& StaticData,
                                               const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
                                               const int YEnd, FVoxelFace* PreviousFaces) const
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);

	auto& PreviousFace =  PreviousFaces[StaticData.FaceDirection];
	if (!PreviousFace.Voxel.IsEmptyVoxel())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		if (!StaticData.RunDirectionFaceMerge(PreviousFace, NewFace))
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, PreviousFace, LocalVoxelTable, StaticData.FaceDirection);
			PreviousFace = NewFace;
		}
	}else
	{
		PreviousFace = NewFace;
	}
}

void URLERunDirectionalVoxelMesher::CreateFrontFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, Y, Z);
	MeshVars.BorderChunks[EFaceDirection::Front][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Top][Index].RunLenght = VoxelGenerator->GetVoxelCountPerChunkLayer();
}

void URLERunDirectionalVoxelMesher::CreateBackFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, Y, Z);
	MeshVars.BorderChunks[EFaceDirection::Back][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Top][Index].RunLenght = VoxelGenerator->GetVoxelCountPerChunkLayer();
}

void URLERunDirectionalVoxelMesher::CreateTopFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, Y, X);
	MeshVars.BorderChunks[EFaceDirection::Top][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Top][Index].RunLenght = VoxelGenerator->GetVoxelCountPerChunkDimension() - Y;
}

void URLERunDirectionalVoxelMesher::CreateBottomFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, Y, X);
	MeshVars.BorderChunks[EFaceDirection::Bottom][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Bottom][Index].RunLenght = VoxelGenerator->GetVoxelCountPerChunkDimension() - Y;
}

void URLERunDirectionalVoxelMesher::CreateRightFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Z);
	MeshVars.BorderChunks[EFaceDirection::Right][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Right][Index].RunLenght = 1;
}

void URLERunDirectionalVoxelMesher::CreateLeftFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Z);
	MeshVars.BorderChunks[EFaceDirection::Left][Index] = Voxel;
	MeshVars.BorderChunks[EFaceDirection::Left][Index].RunLenght = 1;
}