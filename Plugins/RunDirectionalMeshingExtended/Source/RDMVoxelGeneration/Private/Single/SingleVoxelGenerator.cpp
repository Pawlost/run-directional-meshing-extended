#include "Single/SingleVoxelGenerator.h"
#include "VoxelMesher/VirtualChunk.h"

void USingleVoxelGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto VoxelGridDensity = GetVoxelCountPerChunk();
	VoxelModel.Init(VoxelFillIndex, VoxelGridDensity);
}