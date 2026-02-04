#pragma once
#include "VoxelMesher/BasicMesherData.h"

struct FBasicMesherDataDummy : public FBasicMesherData
{
public:
	FBasicMesherDataDummy(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
		: FBasicMesherData(BaseVoxelData)
	{
	}
	
	// workaround to access protected method as public
	using FBasicMesherData::UpdatePositionFromIndex; 
	using FBasicMesherData::UpdateIndexFromPosition; 
	using FBasicMesherData::VoxelPosition; 
	using FBasicMesherData::VoxelIndex;
};
