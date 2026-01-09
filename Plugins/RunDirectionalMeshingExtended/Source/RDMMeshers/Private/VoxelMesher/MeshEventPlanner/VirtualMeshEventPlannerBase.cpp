#include "VoxelMesher/MeshEventPlanner/VirtualMeshEventPlannerBase.h"

void FVirtualMeshEventPlannerBase::TryUpdateNextMeshingEvent(const uint32 EventIndex)
{
	NextMeshingEventIndex = FMath::Min(EventIndex, NextMeshingEventIndex);
}

bool FVirtualMeshEventPlannerBase::AdvanceMeshingEvent(FMeshingEvent& MeshingEvent)
{
	bool EventAdvanced = false;
	int IntervalEventIndex = MeshingEvent.GetEventIndex();
	if (IntervalEventIndex == CurrentMeshingEventIndex)
	{
		// Advance interval in chunk voxel sequence
		MeshingEvent.AdvanceEvent();
		EventAdvanced = true;
	}

	IntervalEventIndex = MeshingEvent.GetEventIndex();
	TryUpdateNextMeshingEvent(IntervalEventIndex);

	return EventAdvanced;
}