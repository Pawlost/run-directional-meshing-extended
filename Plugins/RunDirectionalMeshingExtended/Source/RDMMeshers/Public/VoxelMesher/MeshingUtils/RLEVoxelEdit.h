#pragma once

#include "CoreMinimal.h"
#include "Voxel/RLEVoxel.h"
#include "RLEVoxelEdit.generated.h"

USTRUCT()
struct RDMMESHERS_API FRLEVoxelEdit
{
	GENERATED_BODY()

	uint32 EditEventIndex;
	FRLEVoxel EditVoxel;
};
