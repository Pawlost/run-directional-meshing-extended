#pragma once

#include "VoxelPosition.generated.h"

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGDEMO_API FVoxelPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector ChunkGridPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector VoxelPosition;
};
