#pragma once

#include "CoreMinimal.h"
#include "Voxel/Voxel.h"
#include "RLEVoxel.generated.h"

/**
 * Represents a RLE run in the voxel grid.
 * Contains an ID and a transparency flag.
 */
USTRUCT()
struct RDMMESHERS_API FRLEVoxel
{
	GENERATED_BODY()
	
	uint32 RunLenght = 0;
	FVoxel Voxel;
	
	FORCEINLINE bool IsVoxelEmpty() const
	{
		return Voxel.IsEmptyVoxel();
	}

	FORCEINLINE bool IsTransparent() const
	{
		return Voxel.IsTransparent();
	}

	FORCEINLINE bool IsRunEmpty() const
	{
		return RunLenght == 0;
	}
};
