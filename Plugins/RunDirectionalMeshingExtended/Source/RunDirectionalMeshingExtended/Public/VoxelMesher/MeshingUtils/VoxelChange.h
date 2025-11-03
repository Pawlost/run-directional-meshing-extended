#pragma once

#include "CoreMinimal.h"
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

	void AddVoxelEdit(const FIntVector& VoxelPosition, const FIntVector& ChunkPosition, const FName& VoxelName)
	{
		const FVoxelEdit Modification(VoxelName, VoxelPosition);
		auto& VoxelModifications = VoxelEdits.FindOrAdd(ChunkPosition);
		VoxelModifications.Add(Modification);

		// Voxel modifications must always be sorted using this coordinate logic
		VoxelModifications.Sort([](const FVoxelEdit& A, const FVoxelEdit& B)
		{
			if (A.VoxelPosition.X != B.VoxelPosition.X)
				return A.VoxelPosition.X > B.VoxelPosition.X;
			if (A.VoxelPosition.Z != B.VoxelPosition.Z)
				return A.VoxelPosition.Z > B.VoxelPosition.Z;
			return A.VoxelPosition.Y > B.VoxelPosition.Y;
		});
	}
};
