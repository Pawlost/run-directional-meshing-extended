#pragma once
#include "ChunkEdit.h"
#include "CrossChunkEdit.generated.h"

USTRUCT(BlueprintType)
struct RDMVOXELGENERATION_API FCrossChunkEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FIntVector, FChunkEdit> ChunkEdits;

	FORCEINLINE void AddVoxelEdit(const FIntVector& VoxelPosition, const FIntVector& ChunkPosition, const FName& VoxelName)
	{
		const FVoxelEdit Modification(VoxelName, VoxelPosition);
		auto& VoxelModifications = ChunkEdits.FindOrAdd(ChunkPosition);
		VoxelModifications.VoxelChanges.Add(Modification);
	}
};
