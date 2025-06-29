#include "Voxel/Generator/Single/SingleVoxelGenerator.h"

#include "Chunk/Chunk.h"
#include "VoxelMesher/VoxelMesherBase.h"

void USingleVoxelGenerator::GenerateVoxels(FChunk& Chunk)
{
	const auto VoxelFillIndex = GetSingleVoxel();
	const auto VoxelGridDensity = GetVoxelCountPerChunk();

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.Init(VoxelFillIndex, VoxelGridDensity);
	Chunk.ChunkVoxelIdTable.Add(VoxelFillIndex.VoxelId, VoxelGridDensity);

	VoxelMesher->CompressVoxelGrid(Chunk, VoxelGrid);
}
