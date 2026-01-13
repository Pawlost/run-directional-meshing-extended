#include "Single/FractionFillVoxelGridGenerator.h"

#include "VoxelMesher/VirtualChunk.h"

void UFractionFillVoxelGridGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerVoxelLine();

	VoxelModel.SetNum(GetVoxelCountPerChunk());

	for (uint32 x = 0; x < ChunkDimension / XFraction; x++)
	{
		for (uint32 y = 0; y < ChunkDimension / YFraction; y++)
		{
			for (uint32 z = 0; z < ChunkDimension / ZFraction; z++)
			{
				const auto Index = CalculateVoxelIndex(x, y, z);
				
				VoxelModel[Index] = VoxelFillIndex;
			}
		}
	}
}
