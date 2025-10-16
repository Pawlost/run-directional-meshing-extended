#pragma once
#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"
#include "Chunk.generated.h"

class UVoxelModel;

USTRUCT()
struct RUNDIRECTIONALMESHINGEXTENDED_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;
	
	UPROPERTY()
	TWeakObjectPtr<AChunkActor> BorderChunkMeshActor = nullptr;
	
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT> BorderChunks;

	// Saving voxel grids is not implemented in this demo, but a property specifier for saving has been added.
	UPROPERTY()
	TObjectPtr<UVoxelModel> VoxelModel;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
};