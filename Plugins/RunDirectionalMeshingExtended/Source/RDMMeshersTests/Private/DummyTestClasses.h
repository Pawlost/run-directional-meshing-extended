#pragma once
#include "BaseVoxelData.h"

class UBaseVoxelDataDummy : public UBaseVoxelData
{
public:
	using UBaseVoxelData::CalculateVoxelData; // workaround to access protected method as public
};


class UDummyRLERunDirectionalVoxelMesher //: public URLEVirtualChunk
{
public:
	//using URLERunDirectionalVoxelMesher::AdvanceEditInterval;
	//using URLERunDirectionalVoxelMesher::AdvanceAllMeshingEvents;
	//using URLERunDirectionalVoxelMesher::FaceGeneration;
	//using URLEVirtualChunk::RLEVoxelGrid;
	//using URLERunDirectionalVoxelMesher::TraverseYDirection;
};

