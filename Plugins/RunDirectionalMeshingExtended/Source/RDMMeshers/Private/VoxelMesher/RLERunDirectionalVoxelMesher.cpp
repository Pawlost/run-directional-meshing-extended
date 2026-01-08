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

FVoxel URLERunDirectionalVoxelMesher::GetBorderVoxel(FBorderVirtualMeshEventPlanner& BorderMeshingEventPlanner, const FIntVector& BorderVoxelPosition)
{
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();

	auto& VoxelGridPtr = BorderMeshingEventPlanner.BorderMeshingEvent.VoxelGridPtr;
	if (VoxelGridPtr == nullptr)
	{
		VoxelGridPtr = RLEVoxelGrid;
	}

	uint32 NextIndex = VoxelData->CalculateVoxelIndex(BorderVoxelPosition);
	
	auto& BorderMeshingEvent = BorderMeshingEventPlanner.BorderMeshingEvent;

	while (BorderMeshingEventPlanner.CurrentMeshingEventIndex < NextIndex)
	{
		BorderMeshingEventPlanner.CurrentMeshingEventIndex = BorderMeshingEventPlanner.NextMeshingEventIndex;
		BorderMeshingEventPlanner.NextMeshingEventIndex = MaxChunkVoxelSequence;
		BorderMeshingEventPlanner.AdvanceMeshingEvent(BorderMeshingEvent);
		BorderMeshingEventPlanner.TryUpdateNextMeshingEvent(NextIndex);
	}

	return BorderMeshingEvent.GetCurrentVoxel().Voxel;
}
