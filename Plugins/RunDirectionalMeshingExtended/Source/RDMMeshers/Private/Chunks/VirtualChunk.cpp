#include "VoxelMesher/VirtualChunk/VirtualChunk.h"

#include "BaseVoxelDataDummy.h"

void UVirtualChunk::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
{
	this->VoxelData = VoxelGeneratorBase;
}