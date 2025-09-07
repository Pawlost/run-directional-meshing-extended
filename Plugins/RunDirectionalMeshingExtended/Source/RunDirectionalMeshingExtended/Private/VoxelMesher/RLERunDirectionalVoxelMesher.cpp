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

	IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::Leading] = {
		FirstRun, false, false, FirstRun->RunLenght, 0
	};
	IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::FollowingX] = {
		FirstRun, false, false, FirstRun->RunLenght + SequenceInLayer, 0
	};
	IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::FollowingZ] = {
		FirstRun, false, false, FirstRun->RunLenght + ChunkDimension, 0
	};

	int IntervalEnd = 0;

	// Traverse through voxel grid
	while (IndexParams.CurrentInterval.TraversedVoxelSequence < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.CurrentInterval.TraversedVoxelSequence / (SequenceInLayer);
		const int Z = ((IndexParams.CurrentInterval.TraversedVoxelSequence / ChunkDimension) % ChunkDimension);
		IndexParams.CurrentInterval.Y = IndexParams.CurrentInterval.TraversedVoxelSequence % ChunkDimension;

		do
		{
			if (const auto& LeadingInterval = IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::Leading];
				LeadingInterval.CurrentRun->IsVoxelEmpty() &&
				LeadingInterval.RunEnd == IndexParams.CurrentInterval.TraversedVoxelSequence)
			{
				auto InitialPosition = FIntVector(X, IndexParams.CurrentInterval.Y, Z);
				if (const auto RunIndex = LeadingInterval.RunIndex + 1;
					IndexParams.VoxelGrid->IsValidIndex(RunIndex))
				{
					auto Voxel = GridData[RunIndex];
					// Left
					CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, Voxel,
					           IntervalEnd);
				}
			}

			for (int32 i = 0; i < 3; i++)
			{
				if (IndexParams.CurrentInterval.NextIntervalEnds[i].RunEnd == IndexParams.CurrentInterval.
					TraversedVoxelSequence)
				{
					auto& [CurrentRun, IsOuterInterval, ShowBorders, RunEnd, RunIndex] = IndexParams.CurrentInterval.
						NextIntervalEnds[i];
					RunIndex++;
					CurrentRun = &GridData[RunIndex];
					RunEnd = CurrentRun->RunLenght + IndexParams.CurrentInterval.TraversedVoxelSequence;
					IsOuterInterval = false;
					ShowBorders = false;
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension
			int MinValue = IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::Leading].RunEnd;

			int IntervalFlag = 0;

			for (int32 i = 0; i < 3; i++)
			{
				const auto& Interval = IndexParams.CurrentInterval.NextIntervalEnds[i];

				const int EmptyVoxel = ((Interval.IsOuterInterval && Interval.ShowBorders) || Interval.CurrentRun->
					IsVoxelEmpty());
				IntervalFlag |= EmptyVoxel << i;

				MinValue = FMath::Min(Interval.RunEnd, MinValue);
			}

			// Generate run faces
			auto InitialPosition = FIntVector(X, IndexParams.CurrentInterval.Y, Z);

			if (IntervalFlag != EIntervalType::EmptyFace)
			{
				MinValue = FMath::Min(
					MinValue,
					(ChunkDimension - IndexParams.CurrentInterval.Y) + IndexParams.CurrentInterval.
					TraversedVoxelSequence);

				IntervalEnd = MinValue - IndexParams.CurrentInterval.TraversedVoxelSequence;

				for (const auto& [FaceData, Offset, IntervalEndIndex] : IntervalFaces[IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition + Offset,
					           *IndexParams.CurrentInterval.NextIntervalEnds[IntervalEndIndex].CurrentRun, IntervalEnd);
				}
			}
			else
			{
				IntervalEnd = MinValue - IndexParams.CurrentInterval.TraversedVoxelSequence;
			}

			IndexParams.CurrentInterval.TraversedVoxelSequence = MinValue;
			IndexParams.CurrentInterval.Y += IntervalEnd;

			if (const auto& LeadingInterval = IndexParams.CurrentInterval.NextIntervalEnds[EIntervalEndIndex::Leading];
				!LeadingInterval.CurrentRun->IsVoxelEmpty() && LeadingInterval.RunEnd == IndexParams.CurrentInterval.
				TraversedVoxelSequence)
			{
				// Right
				CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition,
				           *LeadingInterval.CurrentRun, IntervalEnd);
			}
		}
		while (IndexParams.CurrentInterval.Y < ChunkDimension);
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
