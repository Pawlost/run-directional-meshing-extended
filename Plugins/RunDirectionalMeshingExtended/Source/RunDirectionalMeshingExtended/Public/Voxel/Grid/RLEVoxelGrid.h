﻿#pragma once

#include "CoreMinimal.h"
#include "VoxelModel.h"
#include "Voxel/RLEVoxel.h"
#include "RLEVoxelGrid.generated.h"

UCLASS()
class RUNDIRECTIONALMESHINGEXTENDED_API URLEVoxelGrid : public UVoxelModel
{
	GENERATED_BODY()

public:
	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};
