#pragma once

#include "CoreMinimal.h"
#include "VoxelEdit.generated.h"

USTRUCT(BlueprintType)
struct RDMMESHERS_API FVoxelEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FName VoxelName;

	UPROPERTY(EditAnywhere)
	FIntVector VoxelPosition = FIntVector(0);
};
