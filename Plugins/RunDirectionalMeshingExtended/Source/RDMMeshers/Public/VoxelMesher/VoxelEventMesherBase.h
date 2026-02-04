#pragma once
#include "MeshingUtil/MeshingEvent.h"
#include "BasicMesherData.h"

// Must be struct and cannot be class because of UE reflection system limitations
struct FVoxelEventMesherBase : FBasicMesherData
{
	FVoxelEventMesherBase(const TObjectPtr<UBaseVoxelData> BaseVoxelData) : FBasicMesherData(BaseVoxelData)
	{
	}
	
	// return true when interval advanced
	bool AdvanceMeshingEvent(FMeshingEvent& MeshingEvent);
	void TryUpdateNextMeshingEvent(const uint32 EventIndex);
	
	uint32 NextVoxelIndex = 0;
	
protected:
	void Reset();
};
