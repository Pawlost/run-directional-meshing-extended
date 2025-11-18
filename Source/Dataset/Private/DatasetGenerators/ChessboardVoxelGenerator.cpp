#include "DatasetGenerators/ChessboardVoxelGenerator.h"

#include "DatasetGenerators/SphereVoxelGenerator.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UChessboardVoxelGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) 
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerVoxelLine();
	
	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());
	
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 z = 0; z < ChunkDimension; z++)
		{
			for (uint32 y = (z + x)%2; y < ChunkDimension; y+=2)
			{
				const auto Index = CalculateVoxelIndex(x, y, z);
				VoxelModel[Index] = VoxelFillIndex;
			}
		}
	}
}