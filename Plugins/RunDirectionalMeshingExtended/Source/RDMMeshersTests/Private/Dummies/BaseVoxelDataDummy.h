#pragma once
#include "RDMMeshers/Public/BaseVoxelData.h"

class UBaseVoxelDataDummy : public UBaseVoxelData
{
public:
	using UBaseVoxelData::CalculateVoxelData; // workaround to access protected method as public
};
