#pragma once

#include "VoxelChange.generated.h"

USTRUCT(BlueprintType)
struct FVoxelChange
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition = FIntVector(0);
};