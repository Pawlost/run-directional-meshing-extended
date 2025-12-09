#pragma once
#include "Chunk/VirtualVoxelChunk.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"
#include "MesherVariables.generated.h"

class AChunkSpawnerBase;

USTRUCT()
struct RDMVOXELGENERATION_API FMesherVariables
{
	GENERATED_BODY()
	
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TMap<int32, uint32> LocalVoxelTable;
	TMap<int32, uint32> BorderLocalVoxelTable;

	TStaticArray<TSharedPtr<FVirtualVoxelChunk>, CHUNK_FACE_COUNT> SideChunks;
	TSharedPtr<FVirtualVoxelChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	bool ShowBorders = false;
	
	TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData;
	TSharedPtr<TArray<FProcMeshSectionVars>> BorderChunkMeshData;
};
