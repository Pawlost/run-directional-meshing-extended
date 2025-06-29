#pragma once

#include "VoxelChange.generated.h"

USTRUCT()
struct FVoxelChange
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition;
};