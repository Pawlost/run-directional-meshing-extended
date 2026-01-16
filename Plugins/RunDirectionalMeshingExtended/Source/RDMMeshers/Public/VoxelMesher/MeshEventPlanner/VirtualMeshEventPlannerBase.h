#pragma once
#include "MeshingEvent.h"
#include "VoxelIndexCounter.h"

// Must be struct and cannot be class because of UE reflection system limitations
struct FVirtualMeshEventPlannerBase : FVoxelIndexCounter
{
	FVirtualMeshEventPlannerBase(uint32 VoxelLine,uint32 VoxelPlane,uint32 MaxVoxelsInChunk) : FVoxelIndexCounter(VoxelLine, VoxelPlane, MaxVoxelsInChunk)
	{
	}
	
	// return true when interval advanced
	bool AdvanceMeshingEvent(FMeshingEvent& MeshingEvent);
	void TryUpdateNextMeshingEvent(const uint32 EventIndex);
	
	uint32 NextVoxelIndex = 0;
	
protected:
	void Reset();
};
