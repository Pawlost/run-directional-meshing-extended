#pragma once
#include "MeshingUtil/MeshingEvent.h"
#include "VoxelEventMesherBase.h"

struct FBorderEventMesher : FVoxelEventMesherBase
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
	
	FBorderEventMesher() : FVoxelEventMesherBase(nullptr)
	{
	}
};
