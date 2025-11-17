#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Voxel/Voxel.h"
#include "VoxelModel.generated.h"

// Used primarily for casting to derived class
UCLASS()
class RDMMESHERS_API UVoxelModel : public UObject
{
	GENERATED_BODY()

	public:
		virtual FVoxel GetVoxelAtIndex(int32 Index) PURE_VIRTUAL( UVoxelData::GetVoxelAtIndex, return FVoxel(); );
};
