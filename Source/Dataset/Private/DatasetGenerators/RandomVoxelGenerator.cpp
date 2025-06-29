#include "DatasetGenerators/RandomVoxelGenerator.h"

#include "VoxelMesher/VoxelMesherBase.h"

void URandomVoxelGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());

	RandStream.Initialize(Seed);
	
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			for (uint32 z = 0; z < ChunkDimension; z++)
			{
				if(RandStream.FRand() < Threshold)
				{
					FIntVector VoxelPosition(x, y, z);
					const auto Index = CalculateVoxelIndex(VoxelPosition);
					ChangeKnownVoxelAtIndex(VoxelGrid, Chunk.ChunkVoxelIdTable, Index, VoxelFillIndex);
				}
			}
		}
	}

	VoxelMesher->CompressVoxelGrid(Chunk, VoxelGrid);
}
