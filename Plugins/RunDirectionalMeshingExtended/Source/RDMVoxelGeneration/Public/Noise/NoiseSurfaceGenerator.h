#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "NoiseSurfaceGenerator.generated.h"

USTRUCT()
struct RDMVOXELGENERATION_API FNoiseSurfaceGenerator
{
	GENERATED_BODY()
	
	// TODO: rewrite
	//UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> SurfaceGenerator;

	//UPROPERTY()
	TObjectPtr<UFastNoiseWrapper> ReverseSurfaceGenerator;

	FName VoxelName;
	FVoxelTableRow VoxelType;
};
