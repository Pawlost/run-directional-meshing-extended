#pragma once
#include "Chunk/Chunk.h"
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
	TMap<FVoxel, uint32> LocalVoxelTable;
	TMap<FVoxel, uint32> BorderLocalVoxelTable;

	TStaticArray<TSharedPtr<FChunk>, CHUNK_FACE_COUNT> SideChunks;
	TSharedPtr<FChunk> OriginalChunk;

	UPROPERTY()
	TWeakObjectPtr<AActor> SpawnerPtr = nullptr;

	bool WorldTransform = false;
	bool ShowBorders = false;
	
	TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData;
	TSharedPtr<TArray<FProcMeshSectionVars>> BorderChunkMeshData;
};
