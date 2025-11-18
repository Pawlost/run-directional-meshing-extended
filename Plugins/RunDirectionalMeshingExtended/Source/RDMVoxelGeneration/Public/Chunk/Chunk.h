#pragma once
#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelModel/RLEVoxelGrid.h"
#include "Chunk.generated.h"

class UVoxelModel;

USTRUCT()
struct RDMVOXELGENERATION_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;
	
	UPROPERTY()
	TWeakObjectPtr<AChunkActor> BorderChunkMeshActor = nullptr;
	
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT> BorderChunks;

	TStrongObjectPtr<UVoxelModel> VoxelModel;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
};