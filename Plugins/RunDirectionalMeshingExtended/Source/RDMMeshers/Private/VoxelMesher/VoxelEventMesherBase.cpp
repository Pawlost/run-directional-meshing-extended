#include "VoxelMesher/VoxelEventMesherBase.h"

void FVoxelEventMesherBase::TryUpdateNextMeshingEvent(const uint32 EventIndex)
{
	NextVoxelIndex = FMath::Min(EventIndex, NextVoxelIndex);
}

void FVoxelEventMesherBase::Reset()
{
	VoxelIndex = 0;
	NextVoxelIndex = 0;
}

bool FVoxelEventMesherBase::AdvanceMeshingEvent(FMeshingEvent& MeshingEvent)
{
	bool EventAdvanced = false;
	int IntervalEventIndex = MeshingEvent.GetEventIndex();
	if (IntervalEventIndex == VoxelIndex)
	{
		// Advance interval in chunk voxel sequence
		MeshingEvent.AdvanceEvent();
		EventAdvanced = true;
	}

	IntervalEventIndex = MeshingEvent.GetEventIndex();
	TryUpdateNextMeshingEvent(IntervalEventIndex);

	return EventAdvanced;
}
