
#include "DatasetGenerators/SphereVoxelGenerator.h"

#include "DatasetGenerators/ChessboardVoxelGenerator.h"
#include "VoxelMesher/VoxelMesherBase.h"

void USphereVoxelGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());

	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			for (uint32 z = 0; z < ChunkDimension; z++)
			{
				FIntVector VoxelPosition(x, y, z);

				if (FVector::Distance(FVector(ChunkDimension)/2, FVector(VoxelPosition)) < SphereRadius)
				{
					const auto Index = CalculateVoxelIndex(VoxelPosition);
					ChangeKnownVoxelAtIndex(VoxelGrid, Chunk.ChunkVoxelIdTable, Index, VoxelFillIndex);
				}
			}
		}
	}

	VoxelMesher->CompressVoxelGrid(Chunk, VoxelGrid);
}
