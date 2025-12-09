#pragma once
#include "CoreMinimal.h"
#include "ChunkActor.h"
#include "VoxelMesher/VoxelMesherBase.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h" 
#include "VirtualVoxelChunk.generated.h"

class UVoxelModel;

USTRUCT()
struct RDMVOXELGENERATION_API FVirtualVoxelChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;
	
	TStaticArray<TWeakObjectPtr<AChunkActor>, CHUNK_FACE_COUNT> BorderChunkMeshActor;
	
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;

	TStrongObjectPtr<UVoxelMesherBase> VoxelMesher;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
};