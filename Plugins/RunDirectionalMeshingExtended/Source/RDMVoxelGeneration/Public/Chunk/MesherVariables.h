#pragma once
#include "Chunk/Chunk.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RDMVOXELGENERATION_API FMesherVariables
{
	GENERATED_BODY()
	FVoxelMeshContainer MeshContainer;

	TStaticArray<TSharedPtr<FChunk>, VOXEL_FACE_COUNT> SideChunks;
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	// TODO: rewrite or remove
	EBorderVisualizationOption ShowBorders;
};
