#include "Single/SingleVoxelGenerator.h"

void USingleVoxelGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto VoxelGridDensity = GetMaxVoxelsInChunk();
	VoxelModel.Init(VoxelFillIndex, VoxelGridDensity);
}