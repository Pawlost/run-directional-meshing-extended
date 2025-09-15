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

	TFunctionRef<void (FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)> BorderFaces[1] =
	{
		[this](FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
		{
			this->CreateFrontFace(MeshVars, X, Y, Z, Voxel);
		}
	};
	
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

	// Traverse through voxel grid
	while (IndexParams.TraversedVoxelSequence < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.TraversedVoxelSequence / (SequenceInLayer);
		const int Z = ((IndexParams.TraversedVoxelSequence / ChunkDimension) % ChunkDimension);
		int Y = IndexParams.TraversedVoxelSequence % ChunkDimension;
		
		// Check borders
		//TODO: uncomment
        uint8 BorderMask = 0;
        BorderMask |= (X == ChunkDimension-1) << EFaceDirection::Front;
		//   BorderMask |= (X == 0) << EFaceDirection::Back;
		//   BorderMask |= (IntervalEnd + Y == ChunkDimension) << EFaceDirection::Right;
		//	BorderMask |= (Y == 0) << EFaceDirection::Left;
        // BorderMask |= (Z == 0) << EFaceDirection::Bottom;
        // BorderMask |= (Z == ChunkDimension-1) << EFaceDirection::Top;
		
		do
		{
			// Move interval one step ahead if at the run end
			if (auto& Interval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading]; Interval.RunEnd == IndexParams.TraversedVoxelSequence)
			{
				const FRLEVoxel* PreviousRun = Interval.CurrentRun;
				
				Interval.RunIndex++;
				Interval.CurrentRun = &GridData[Interval.RunIndex];
				Interval.RunEnd = Interval.CurrentRun->RunLenght + IndexParams.TraversedVoxelSequence;

				if (PreviousRun->Voxel.IsEmptyVoxel() && Y != 0)
				{
					InitialPosition = FIntVector(X, Y, Z);
					// Left
					CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, *Interval.CurrentRun, IntervalEnd, PreviousFaces);
				}
				else if (Interval.CurrentRun->Voxel.IsEmptyVoxel() && InitialPosition.Y + IntervalEnd != ChunkDimension )
				{
					// Right
					CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition, *PreviousRun, IntervalEnd, PreviousFaces);
				}
			}
	
			for (int32 i = 1; i < 3; i++)
			{
				// Move interval one step ahead if at the run end
				if (auto& Interval = IndexParams.NextIntervalEnds[i]; Interval.RunEnd == IndexParams.TraversedVoxelSequence)
				{
					Interval.RunIndex++;
					Interval.CurrentRun = &GridData[Interval.RunIndex];
					Interval.RunEnd = Interval.CurrentRun->RunLenght + IndexParams.TraversedVoxelSequence;
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension
			int MinValue = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading].RunEnd;

			uint8 IntervalFlag = 0;

			for (int32 i = 0; i < 3; i++)
			{
				const auto& Interval = IndexParams.NextIntervalEnds[i];

				const int EmptyVoxel = Interval.CurrentRun->IsVoxelEmpty();
				// Create mask based on interval combination, the mask is used as an index to array to prevent branching
				// Indexes are described in github documentation
				IntervalFlag |= EmptyVoxel << i;

				MinValue = FMath::Min(Interval.RunEnd, MinValue);
			}
			
			// Prevents Z following at the bottom of a chunk traversal (otherwise bottom follows top at x - 1)
			// Achieved by setting FollowingZ to Leading interval flag without changing value of the other intervals
			const bool ZIntervalToggle = (Z == 0) & ((IntervalFlag & 1) != (IntervalFlag >> EIntervalEndIndex::FollowingZ));
			IntervalFlag ^= ZIntervalToggle << EIntervalEndIndex::FollowingZ;
			
			// Generate culled faces
			if (IntervalFlag != EIntervalType::EmptyFaceInterval)
			{
				MinValue = FMath::Min(MinValue,(ChunkDimension - Y) + IndexParams.TraversedVoxelSequence);

				IntervalEnd = MinValue - IndexParams.TraversedVoxelSequence;

				InitialPosition = FIntVector(X, Y, Z);
				
				const auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];

				if (BorderMask != 0 && !LeadingInterval.CurrentRun->IsVoxelEmpty())
				{
					for (int i = 0; i < 6; i++)
					{
						if (BorderMask & (1 << i))
						{
							BorderFaces[i](MeshVars, X, Y, Z, *IndexParams.NextIntervalEnds[Leading].CurrentRun);
						}
					}
				}
				
				// Inner culling
				for (const auto& [FaceData, Offset, IntervalEndIndex] : IntervalFaces[IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition + Offset,
							   *IndexParams.NextIntervalEnds[IntervalEndIndex].CurrentRun, IntervalEnd, PreviousFaces);
				}
			}
			else
			{
				IntervalEnd = MinValue - IndexParams.TraversedVoxelSequence;
			}

			IndexParams.TraversedVoxelSequence = MinValue;
			Y += IntervalEnd;
		} while (Y < ChunkDimension);
	}

	for (int direction = 0; direction < CHUNK_FACE_COUNT; direction++)
	{
		auto& PreviousFace = PreviousFaces[direction];
		if (!PreviousFace.Voxel.IsEmptyVoxel())
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, PreviousFace, LocalVoxelTable, direction);
		}

		for (int i = 0; i < SequenceInLayer; i++)
		{
			const int Z = ((i / ChunkDimension) % ChunkDimension);
			int Y = i % ChunkDimension;
			
			InitialPosition = FIntVector(ChunkDimension-1, Y , Z);
			auto Voxel = MeshVars.BorderChunks[EFaceDirection::Front][i];
			if (!Voxel.IsVoxelEmpty()){
				const FVoxelFace NewFace = FrontFaceTemplate.StaticMeshingData.FaceCreator(Voxel.Voxel, InitialPosition, 1);
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, direction);
			}
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
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Front][Index] = Voxel;
}

void URLERunDirectionalVoxelMesher::CreateBackFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Back][Index] = Voxel;
}

void URLERunDirectionalVoxelMesher::CreateTopFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Top][Index] = Voxel;
}

void URLERunDirectionalVoxelMesher::CreateBottomFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Bottom][Index] = Voxel;
}

void URLERunDirectionalVoxelMesher::CreateRightFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Right][Index] = Voxel;
}

void URLERunDirectionalVoxelMesher::CreateLeftFace(FMesherVariables& MeshVars, int X, int Y, int Z, const FRLEVoxel& Voxel)
{
	auto Index = VoxelGenerator->CalculateVoxelIndex(0, X, Y);
	MeshVars.BorderChunks[EFaceDirection::Left][Index] = Voxel;
}