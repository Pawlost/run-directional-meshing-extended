#pragma once

#include "CoreMinimal.h"
#include "Voxel/VoxelPosition.h"
#include "VoxelChange.generated.h"

USTRUCT(BlueprintType)
struct FVoxelEdit
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition = FIntVector(0);
};

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGEXTENDED_API FCrossChunkEdit
{
	GENERATED_BODY()
	
	TMap<FIntVector, TArray<FVoxelEdit>> VoxelEdits;

	void AddVoxelEdit(const FVoxelPosition& VoxelPosition, const FName& VoxelName)
	{
		const FVoxelEdit Modification(VoxelName, VoxelPosition.VoxelPosition);
		VoxelEdits.FindOrAdd(VoxelPosition.ChunkGridPosition).Add(Modification);
	}
};