#pragma once
#include "CoreMinimal.h"
#include "VoxelMesher/MeshingUtil/VirtualVoxelFace.h"
#include "VoxelMesher/MeshingUtil/Enum/FaceDirection.h"
#include "VoxelModel/RLEVoxelGrid.h"
#include "Chunk.generated.h"

class UVirtualChunk;
class AChunkActor;
class UVoxelModel;

USTRUCT()
struct RDMVOXELGENERATION_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;
	
	TStaticArray<TWeakObjectPtr<AChunkActor>, VOXEL_FACE_COUNT> BorderChunkMeshActor;
	
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, VOXEL_FACE_COUNT> VirtualFaces;

	TStrongObjectPtr<UVoxelModel> VoxelModel;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
	
	UPROPERTY()
	TObjectPtr<UVirtualChunk> VoxelMesher;
};