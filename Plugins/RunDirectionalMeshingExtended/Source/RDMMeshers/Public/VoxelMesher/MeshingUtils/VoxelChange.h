#pragma once

#include "CoreMinimal.h"
#include "VoxelChange.generated.h"

USTRUCT(BlueprintType)
struct RDMMESHERS_API FVoxelChange
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition = FIntVector(0);
};
