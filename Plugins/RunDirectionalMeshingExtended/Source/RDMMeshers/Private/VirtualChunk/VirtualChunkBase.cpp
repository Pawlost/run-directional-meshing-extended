#include "VirtualChunk/VirtualChunkBase.h"

#include "BaseVoxelData.h"

void UVirtualChunkBase::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
{
	this->VoxelData = VoxelGeneratorBase;
}
