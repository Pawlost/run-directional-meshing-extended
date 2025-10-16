#pragma once

#include "VoxelPosition.generated.h"

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGEXTENDED_API FVoxelPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector ChunkGridPosition = FIntVector(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FIntVector VoxelPosition = FIntVector(0);
};
