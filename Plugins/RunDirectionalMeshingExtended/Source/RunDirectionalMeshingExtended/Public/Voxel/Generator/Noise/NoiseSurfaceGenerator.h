#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "Voxel/VoxelType.h"
#include "NoiseSurfaceGenerator.generated.h"

USTRUCT()
struct RUNDIRECTIONALMESHINGEXTENDED_API FNoiseSurfaceGenerator
{
	GENERATED_BODY()

	//PROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceGenerator;

	//UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> ReverseSurfaceGenerator;

	FName VoxelName;
	FVoxelType VoxelType;
};
