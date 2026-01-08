#pragma once

struct FVirtualMeshEventPlannerBase
{
	// return true when interval advanced
	bool AdvanceMeshingEvent(FMeshingEvent& MeshingEvent);
	void TryUpdateNextMeshingEvent(const uint32 EventIndex);
	
	// Current event index made of all meshing events that were already processed/traversed.
	uint32 CurrentMeshingEventIndex = 0;
	uint32 NextMeshingEventIndex = 0;
};
