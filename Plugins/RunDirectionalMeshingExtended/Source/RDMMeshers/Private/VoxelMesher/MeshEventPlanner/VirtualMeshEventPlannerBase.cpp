#include "VoxelMesher/MeshEventPlanner/VirtualMeshEventPlannerBase.h"

void FVirtualMeshEventPlannerBase::TryUpdateNextMeshingEvent(const uint32 EventIndex)
{
	NextVoxelIndex = FMath::Min(EventIndex, NextVoxelIndex);
}

void FVirtualMeshEventPlannerBase::Reset()
{
	CurrentVoxelIndex = 0;
	NextVoxelIndex = 0;
}

bool FVirtualMeshEventPlannerBase::AdvanceMeshingEvent(FMeshingEvent& MeshingEvent)
{
	bool EventAdvanced = false;
	int IntervalEventIndex = MeshingEvent.GetEventIndex();
	if (IntervalEventIndex == CurrentVoxelIndex)
	{
		// Advance interval in chunk voxel sequence
		MeshingEvent.AdvanceEvent();
		EventAdvanced = true;
	}

	IntervalEventIndex = MeshingEvent.GetEventIndex();
	TryUpdateNextMeshingEvent(IntervalEventIndex);

	return EventAdvanced;
}
