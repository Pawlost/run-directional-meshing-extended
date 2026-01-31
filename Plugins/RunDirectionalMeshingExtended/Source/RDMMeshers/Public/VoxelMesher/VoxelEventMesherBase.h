#pragma once
#include "MeshingEvent.h"
#include "BasicMesherData.h"

// Must be struct and cannot be class because of UE reflection system limitations
struct FVoxelEventMesherBase : FBasicMesherData
{
	FVoxelEventMesherBase(const uint32 VoxelLine, const uint32 VoxelPlane, const uint32 MaxVoxelsInChunk) : FBasicMesherData(VoxelLine, VoxelPlane, MaxVoxelsInChunk)
	{
	}
	
	// return true when interval advanced
	bool AdvanceMeshingEvent(FMeshingEvent& MeshingEvent);
	void TryUpdateNextMeshingEvent(const uint32 EventIndex);
	
	uint32 NextVoxelIndex = 0;
	
protected:
	void Reset();
};
