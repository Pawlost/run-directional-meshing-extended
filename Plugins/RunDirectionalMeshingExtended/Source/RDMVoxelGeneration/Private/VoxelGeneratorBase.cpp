#include "VoxelGeneratorBase.h"

#include "Chunk/MesherVariables.h"
#include "VoxelMesher/VirtualChunk.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/RLEVoxelEdit.h"

void UVoxelGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
}

void UVoxelGeneratorBase::GenerateVoxels(FChunk& Chunk)
{
	FScopeLock Lock(&Mutex);
	TArray<FVoxel> VoxelModelArray;
	AddVoxels(Chunk, VoxelModelArray);
	Chunk.VoxelMesher->CompressVoxelModel( VoxelModelArray);
}