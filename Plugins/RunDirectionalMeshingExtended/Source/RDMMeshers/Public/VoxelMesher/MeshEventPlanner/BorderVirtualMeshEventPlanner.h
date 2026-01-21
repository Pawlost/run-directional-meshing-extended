#pragma once
#include "MeshingEvent.h"
#include "VirtualMeshEventPlannerBase.h"

struct FBorderVirtualMeshEventPlanner : FVirtualMeshEventPlannerBase
{
	FMeshingEvent BorderMeshingEvent;
	FORCEINLINE void SetVoxelIndex(uint32 NewVoxelIndex)
	{
		VoxelIndex = NewVoxelIndex;
	}
	
	FORCEINLINE uint32 GetVoxelIndex() const
	{
		return VoxelIndex;
	}
	
	FBorderVirtualMeshEventPlanner() : FVirtualMeshEventPlannerBase(0, 0, 0)
	{
	}
};
