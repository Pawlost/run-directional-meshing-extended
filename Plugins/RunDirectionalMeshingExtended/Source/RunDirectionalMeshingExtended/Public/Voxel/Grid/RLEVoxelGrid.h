#pragma once

#include "CoreMinimal.h"
#include "VoxelModel.h"
#include "Voxel/RLEVoxel.h"
#include "RLEVoxelGrid.generated.h"

USTRUCT()
struct FBorderChunk
{
	GENERATED_BODY()
	TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples;
	TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples;

	FBorderChunk(){}

	explicit FBorderChunk(const int VoxelLayer)
	{
		BorderVoxelSamples = MakeShared<TArray<FRLEVoxel>>();
		InversedBorderVoxelSamples = MakeShared<TArray<FRLEVoxel>>();
		BorderVoxelSamples->SetNum(VoxelLayer);
		InversedBorderVoxelSamples->SetNum(VoxelLayer);
	}

	bool IsSampled = false;
	bool IsInverseSampled = false;
	bool IsGenerated = false;
};

UCLASS()
class RUNDIRECTIONALMESHINGEXTENDED_API URLEVoxelGrid : public UVoxelModel
{
	GENERATED_BODY()

public:
	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};