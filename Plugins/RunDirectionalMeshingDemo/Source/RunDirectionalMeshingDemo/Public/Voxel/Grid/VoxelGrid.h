#pragma once

#include "CoreMinimal.h"
#include "VoxelModel.h"
#include "UObject/Object.h"
#include "Voxel/Voxel.h"
#include "VoxelGrid.generated.h"

UCLASS()
class RUNDIRECTIONALMESHINGDEMO_API UVoxelGrid : public UVoxelModel
{
	GENERATED_BODY()

public:
	TSharedPtr<TArray<FVoxel>> VoxelGrid;

	virtual FVoxel GetVoxelAtIndex(const int32 Index) override
	{
		return (*VoxelGrid)[Index];		
	}
};
