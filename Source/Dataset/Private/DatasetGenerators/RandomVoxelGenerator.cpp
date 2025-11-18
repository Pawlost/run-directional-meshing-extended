#include "DatasetGenerators/RandomVoxelGenerator.h"

#include "VoxelMesher/VoxelMesherBase.h"

void URandomVoxelGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) 
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerVoxelLine();

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());

	RandStream.Initialize(Seed);
	
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			for (uint32 z = 0; z < ChunkDimension; z++)
			{
				if(RandStream.FRandRange(0, 1) < Threshold)
				{
					FIntVector VoxelPosition(x, y, z);
					const auto Index = CalculateVoxelIndex(VoxelPosition);
					VoxelModel[Index] = VoxelFillIndex;
				}
			}
		}
	}
}