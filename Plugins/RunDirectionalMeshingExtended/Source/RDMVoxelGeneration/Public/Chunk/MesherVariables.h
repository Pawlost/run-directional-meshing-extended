#pragma once
#include "Chunk/Chunk.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RDMVOXELGENERATION_API FChunkParams
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
struct RDMVOXELGENERATION_API FMesherVariables
{
	GENERATED_BODY()
	
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TMap<int32, uint32> LocalVoxelTable;
	TMap<int32, uint32> BorderLocalVoxelTable;

	FChunkParams ChunkParams;
	TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData;
	TSharedPtr<TArray<FProcMeshSectionVars>> BorderChunkMeshData;
};
