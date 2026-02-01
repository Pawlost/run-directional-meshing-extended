#include "VoxelMesher/VirtualChunk/VirtualChunk.h"

#include "BaseVoxelData.h"

void UVirtualChunk::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
{
	this->VoxelData = VoxelGeneratorBase;
}