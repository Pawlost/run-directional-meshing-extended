#pragma once
#include "FaceDirection.h"
#include "Chunk/Chunk.h"
#include "VoxelMesher/MeshingUtils/VoxelFace.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunk> SideChunks[CHUNK_FACE_COUNT];
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AChunkSpawnerBase> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	bool ShowBorders = false;
	bool ExecutedOnMainThread = false;
};

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FMesherVariables
{
	GENERATED_BODY()

	TArray<TSharedPtr<TArray<FVoxelFace>>> Faces[CHUNK_FACE_COUNT];
	FChunkParams ChunkParams;
	TMap<uint16, uint16> VoxelIdToLocalVoxelMap; 
};
