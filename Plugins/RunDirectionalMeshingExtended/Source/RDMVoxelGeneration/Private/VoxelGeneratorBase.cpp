#include "VoxelGeneratorBase.h"

#include "Chunk/MesherVariables.h"
#include "VoxelMesher/VoxelMesherBase.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/RLEVoxelEdit.h"

void UVoxelGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
}

void UVoxelGeneratorBase::GenerateVoxels(FChunk& Chunk)
{
	FScopeLock Lock(&Mutex);
	AddVoxels(Chunk, Chunk.VoxelModelArray);
}