#pragma once
#include "BaseVoxelData.h"
#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

class UBaseVoxelDataDummy : public UBaseVoxelData
{
public:
	using UBaseVoxelData::CalculateVoxelData; // workaround to access protected method as public
};


class UDummyRLERunDirectionalVoxelMesher : public URLERunDirectionalVoxelMesher
{
public:
	using URLERunDirectionalVoxelMesher::AdvanceEditInterval;
	using URLERunDirectionalVoxelMesher::AdvanceAllMeshingEvents;
};

