#pragma once

#include "CoreMinimal.h"
#include "VoxelChange.generated.h"

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGEXTENDED_API FVoxelEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition = FIntVector(0);
};

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGEXTENDED_API FChunkEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVoxelEdit> VoxelEdits;

	void Add(const FVoxelEdit& VoxelEdit)
	{
		VoxelEdits.Add(VoxelEdit);
	}
};


USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGEXTENDED_API FCrossChunkEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntVector, FChunkEdit> ChunkEdits;

	void AddVoxelEdit(const FIntVector& VoxelPosition, const FIntVector& ChunkPosition, const FName& VoxelName)
	{
		const FVoxelEdit Modification(VoxelName, VoxelPosition);
		auto& VoxelModifications = ChunkEdits.FindOrAdd(ChunkPosition);
		VoxelModifications.Add(Modification);
	}
};
