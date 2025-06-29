#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "Voxel/VoxelType.h"
#include "NoiseSurfaceGenerator.generated.h"

USTRUCT()
struct RUNDIRECTIONALMESHINGDEMO_API FNoiseSurfaceGenerator
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceGenerator;

	UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> ReverseSurfaceGenerator;

	FName VoxelName;
	FVoxelType VoxelType;
};
