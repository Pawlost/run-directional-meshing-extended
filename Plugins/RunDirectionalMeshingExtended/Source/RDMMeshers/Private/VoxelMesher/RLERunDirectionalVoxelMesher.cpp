#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"

void URLERunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	if (VoxelGrid.Num() != 0 && RLEVoxelGrid == nullptr)
	{
		auto NewRLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

		// Seed with first voxel run
		NewRLEVoxelGrid->Emplace(1, VoxelGrid[0]);

		for (int32 x = 1; x < VoxelGrid.Num(); x++)
		{
			const FVoxel& Voxel = VoxelGrid[x];
			if (NewRLEVoxelGrid->Last().Voxel == Voxel)
			{
				NewRLEVoxelGrid->Last().RunLenght++;
			}
			else
			{
				NewRLEVoxelGrid->Emplace(1, Voxel);
			}
		}

		RLEVoxelGrid = NewRLEVoxelGrid;
	}
}

// persistent preallocation must be maintained
void URLERunDirectionalVoxelMesher::GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
                                                 TArray<FRLEVoxelEdit>& VoxelEdits, bool ShowBorders)
{
	// This scope may start in a parallel task
	
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif
	
	const uint32 MaxVoxelsInChunk = VoxelData->GetVoxelCountPerChunk();
	const uint32 VoxelLine = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlane = VoxelData->GetVoxelCountPerVoxelPlane();

	// Keeps all variables local inside the task scope
	FVirtualMeshEventPlanner EventPlanner(VoxelLine, VoxelPlane, MaxVoxelsInChunk, ShowBorders);
	EventPlanner.InitializeIntervals(RLEVoxelGrid, VoxelEdits);
	EventPlanner.GenerateVirtualFaces(BorderParameters, VoxelEdits);
	
	if (EventPlanner.IsEditEnabled())
	{
		RLEVoxelGrid = EventPlanner.GetMainVoxelGridPtr();
	}
	
	EventPlanner.DirectionalGreedyMerge(MeshContainer, VoxelData->VoxelSize);
}

FVoxel URLERunDirectionalVoxelMesher::GetBorderVoxel(FSimpleVirtualVoxelFace& IndexParams, FIntVector VoxelPosition)
{
	/*
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();

	auto& VoxelGridPtr = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	if (VoxelGridPtr == nullptr)
	{
		VoxelGridPtr = RLEVoxelGrid;
	}

	uint32 NextIndex = VoxelData->CalculateVoxelIndex(X, Y, Z);

	while (IndexParams.CurrentMeshingEventIndex < NextIndex)
	{
		IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
		IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;
		AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval);
		IndexParams.TryUpdateNextMeshingEvent(NextIndex);
	}

	return IndexParams.MeshingEvents[LeadingInterval].GetCurrentVoxel().Voxel;
*/
	return FVoxel();
}
