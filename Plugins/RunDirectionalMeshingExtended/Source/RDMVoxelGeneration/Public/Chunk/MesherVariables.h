#pragma once
#include "Chunk/Chunk.h"
#include "VoxelMesher/MeshingUtil/VoxelMesh.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RDMVOXELGENERATION_API FMesherVariables
{
	GENERATED_BODY()
	FVoxelMesh MeshContainer;

	TStaticArray<TSharedPtr<FChunk>, VOXEL_FACE_COUNT> SideChunks;
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnerPtr = nullptr;

	bool WorldTransform = false;
};
