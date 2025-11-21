#pragma once

#include "CoreMinimal.h"
#include "Voxel/RLEVoxel.h"
#include "BorderChunk.generated.h"


USTRUCT()
struct RDMMESHERS_API FBorderChunk
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