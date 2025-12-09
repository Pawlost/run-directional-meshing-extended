#include "VoxelGeneratorBase.h"

#include "Chunk/MesherVariables.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UVoxelGeneratorBase::AddVoxelsToChunk(const FVirtualVoxelChunk& Chunk)
{
	FScopeLock Lock(&Mutex);
	Chunk.VoxelMesher->SetVoxelGenerator(this);
	TArray<FVoxel> VoxelModelArray;
	GenerateVoxels(Chunk, VoxelModelArray);
	Chunk.VoxelMesher->AddUncompressedGrid(VoxelModelArray);
}