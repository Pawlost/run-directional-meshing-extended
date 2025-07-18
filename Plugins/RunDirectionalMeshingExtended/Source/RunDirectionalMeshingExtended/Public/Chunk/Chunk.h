﻿#pragma once
#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "Chunk.generated.h"

class UVoxelModel;

USTRUCT()
struct RUNDIRECTIONALMESHINGEXTENDED_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;

	// Saving voxel grids is not implemented in this demo, but a property specifier for saving has been added.

	UPROPERTY()
	TObjectPtr<UVoxelModel> VoxelModel;

	//Key: voxel ID; Value: voxel count
	UPROPERTY()
	TMap<int32, uint32> ChunkVoxelIdTable;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bHasMesh = false;

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
};
