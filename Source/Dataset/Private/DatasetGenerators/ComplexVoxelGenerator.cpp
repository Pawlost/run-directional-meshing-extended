#include "DatasetGenerators/ComplexShapeVoxelGenerator.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UComplexShapeVoxelGenerator::AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerVoxelLine();
	
	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());
	
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			for (uint32 z = 0; z < ChunkDimension; z++)
			{
				// This function was taken from: https://0fps.net/2012/07/07/meshing-minecraft-part-2/
				
				if (ShapeSinus(x) + ShapeSinus(y) + ShapeSinus(z) > 0)
				{
					const auto Index = CalculateVoxelIndex(x, y, z);
					VoxelModel[Index] = VoxelFillIndex;
				}
			}
		}
	}
}

float UComplexShapeVoxelGenerator::ShapeSinus(const int Coordinate) const
{
	return FMath::Sin((0.5 * Omega * Coordinate)/PI );
}