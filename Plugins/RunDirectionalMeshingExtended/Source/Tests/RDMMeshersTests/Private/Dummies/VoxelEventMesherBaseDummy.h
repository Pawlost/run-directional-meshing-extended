#pragma once
#include "VoxelMesher/VoxelEventMesherBase.h"

struct FVoxelEventMesherBaseDummy : public FVoxelEventMesherBase
{
public:
	FVoxelEventMesherBaseDummy(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
		: FVoxelEventMesherBase(BaseVoxelData)
	{
	}

	// workaround to access protected method as public
	using FVoxelEventMesherBase::Reset;
	using FBasicMesherData::VoxelIndex;
};
