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
	IntervalFaces[EIntervalType::FullCulledFace] = {};

	// FrontTopFace
	IntervalFaces[EIntervalType::FrontTopFace] = {
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX},
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BackFace
	IntervalFaces[EIntervalType::BackFace] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// TopFace
	IntervalFaces[EIntervalType::TopFace] = {
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BottomFace
	IntervalFaces[EIntervalType::BottomFace] = {
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// FrontFace
	IntervalFaces[EIntervalType::FrontFace] = {
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX}
	};

	// BackBottomFace
	IntervalFaces[EIntervalType::BackBottomFace] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading},
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// EmptyFace
	IntervalFaces[EIntervalType::EmptyFace] = {};
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, const FMesherVariables& MeshVars,
                                                   TMap<uint32, uint32>& LocalVoxelTable) const
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

	IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading] = {FirstRun, FirstRun->RunLenght, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingX] = {FirstRun,  FirstRun->RunLenght + SequenceInLayer, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingZ] = {FirstRun, FirstRun->RunLenght + ChunkDimension, 0 };

	int IntervalEnd = 0;

	// Traverse through voxel grid
	while (IndexParams.TraversedVoxelSequence < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.TraversedVoxelSequence / (SequenceInLayer);
		const int Z = ((IndexParams.TraversedVoxelSequence / ChunkDimension) % ChunkDimension);
		int Y = IndexParams.TraversedVoxelSequence % ChunkDimension;
		
		do
		{
			// Generate Left face, it is culled by traversed sequence
			if (const auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];
				LeadingInterval.CurrentRun->IsVoxelEmpty() &&
				LeadingInterval.RunEnd == IndexParams.TraversedVoxelSequence)
			{
				auto InitialPosition = FIntVector(X, Y, Z);
				if (const auto RunIndex = LeadingInterval.RunIndex + 1;
					IndexParams.VoxelGrid->IsValidIndex(RunIndex))
				{
					auto Voxel = GridData[RunIndex];
					// Left
					CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, Voxel, IntervalEnd);
				}
			}
	
			for (int32 i = 0; i < 3; i++)
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
			if (IntervalFlag != EIntervalType::EmptyFace)
			{
				MinValue = FMath::Min(MinValue,(ChunkDimension - Y) + IndexParams.TraversedVoxelSequence);

				IntervalEnd = MinValue - IndexParams.TraversedVoxelSequence;

				auto InitialPosition = FIntVector(X, Y, Z);
				
				const auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];
				
				uint8 BorderMask = 0;
				BorderMask |= (X == ChunkDimension-1) << EFaceDirection::Front;
				BorderMask |= (X == 0) << EFaceDirection::Back;
				BorderMask |= (IntervalEnd == ChunkDimension) << EFaceDirection::Right;
				BorderMask |= (Y == 0) << EFaceDirection::Left;
				BorderMask |= (Z == 0) << EFaceDirection::Bottom;
				BorderMask |= (Z == ChunkDimension-1) << EFaceDirection::Top;

				if (BorderMask != 0 && !LeadingInterval.CurrentRun->IsVoxelEmpty())
				{
					for (int i = 0; i < 6; i++)
					{
						if (BorderMask & (1 << i))
						{
							CreateFace(MeshVars, LocalVoxelTable, BorderFaces[i],
										   InitialPosition, *LeadingInterval.CurrentRun, IntervalEnd);
						}
					}
				}
				
				// Inner
				for (const auto& [FaceData, Offset, IntervalEndIndex] : IntervalFaces[IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition + Offset,
							   *IndexParams.NextIntervalEnds[IntervalEndIndex].CurrentRun, IntervalEnd);
				}
			}
			else
			{
				IntervalEnd = MinValue - IndexParams.TraversedVoxelSequence;
			}

			IndexParams.TraversedVoxelSequence = MinValue;

			// Generate Right face, it is culled by traversed sequence
			if (const auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];
				!LeadingInterval.CurrentRun->IsVoxelEmpty() && LeadingInterval.RunEnd == IndexParams.TraversedVoxelSequence)
			{
				auto InitialPosition = FIntVector(X, Y, Z);
				// Right
				CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition,
						   *LeadingInterval.CurrentRun, IntervalEnd);
			}

			Y += IntervalEnd;
			
		} while (Y < ChunkDimension);
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
                                               const int YEnd) const
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	const auto FaceIndex = static_cast<uint8>(StaticData.FaceDirection);

	ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, FaceIndex);
}
