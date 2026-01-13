#pragma once
#include "CoreMinimal.h"
#include "VoxelMesher/RLEVirtualChunk.h"
#include "VoxelMesher/VirtualChunk.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h"
#include "VoxelModel/RLEVoxelGrid.h"
#include "Chunk.generated.h"

class AChunkActor;
class UVoxelModel;

USTRUCT()
struct RDMVOXELGENERATION_API FChunk
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<AChunkActor> ChunkMeshActor = nullptr;
	
	TStaticArray<TWeakObjectPtr<AChunkActor>, CHUNK_FACE_COUNT> BorderChunkMeshActor;
	
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;

	TStrongObjectPtr<UVoxelModel> VoxelModel;

	UPROPERTY()
	FIntVector GridPosition = FIntVector(0, 0, 0);

	UPROPERTY(VisibleInstanceOnly)
	bool bIsActive = false;
	
	UPROPERTY()
	TObjectPtr<UVirtualChunk> VoxelMesher;
	
	void InitVoxelMesher(const TSubclassOf<UVirtualChunk>& VoxelMesherBlueprint, const TObjectPtr<UBaseVoxelData>&  BaseVoxelData)
	{
		checkf(VoxelMesherBlueprint, TEXT("Mesher blueprint must be set"));
		if (VoxelMesherBlueprint)
		{
			// Register mesher
			VoxelMesher = NewObject<URLEVirtualChunk>(VoxelMesherBlueprint);
			VoxelMesher->SetVoxelGenerator(BaseVoxelData);
		}
	}
};