#include "DatasetGenerators/StripsVoxelGenerator.h"

#include "VoxelMesher/VoxelMesherBase.h"

bool UStripsVoxelGenerator::IsInGap(const uint32 Coordinate, const int32 StripDimension)
{
	return (Coordinate / StripDimension) % 2 != 0 ;
}

uint32 UStripsVoxelGenerator::CalculateStripSize(const float ChunkDimension, const int32 GapCount)
{
	// Last strip should be filled to atleast 75% or decrease strip size 
	const float StripSize = ChunkDimension/(GapCount + GapCount + 0.75f);
	return FMath::Max<uint32>(FMath::FloorToInt32(StripSize), 1);
}

void UStripsVoxelGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());

	const uint32 StripSizeX = CalculateStripSize(ChunkDimension, XGapCount);
	const uint32 StripSizeY = CalculateStripSize(ChunkDimension, YGapCount); 
	const uint32 StripSizeZ = CalculateStripSize(ChunkDimension, ZGapCount);

	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			for (uint32 z = 0; z < ChunkDimension; z++)
			{
				FIntVector VoxelPosition(x, y, z);
				if (IsInGap(x, StripSizeX) || IsInGap(y, StripSizeY) || IsInGap(z, StripSizeZ))
				{
					continue;
				}
				
				const auto Index = CalculateVoxelIndex(VoxelPosition);
				ChangeKnownVoxelAtIndex(VoxelGrid, Chunk.ChunkVoxelIdTable, Index, VoxelFillIndex);
			}
		}
	}

	VoxelMesher->CompressVoxelGrid(Chunk, VoxelGrid);
}
