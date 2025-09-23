#pragma once
#include "FaceDirection.h"
#include "Chunk/Chunk.h"
#include "VoxelMesher/MeshingUtils/VoxelFace.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RUNDIRECTIONALMESHINGEXTENDED_API FChunkParams
{
	GENERATED_BODY()

	TSharedPtr<FChunk> SideChunks[CHUNK_FACE_COUNT];
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AChunkSpawnerBase> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	bool ShowBorders = false;
};

USTRUCT()
struct RUNDIRECTIONALMESHINGEXTENDED_API FMesherVariables
{
	GENERATED_BODY()
	
	TSharedPtr<TArray<FVoxelFace>> Faces[CHUNK_FACE_COUNT];
	FChunkParams ChunkParams;
	TSharedPtr<TArray<FProcMeshSectionVars>> QuadMeshSectionArray;
};
