#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Voxel/Generator/Single/SingleVoxelGeneratorBase.h"
#include "StripsVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UStripsVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()
	
public:
	virtual void GenerateVoxels(FChunk& Chunk) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"), Category = "Voxel")
	int32 XGapCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"), Category = "Voxel")
	int32 YGapCount = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0"), Category = "Voxel")
	int32 ZGapCount = 1;
private:
	static bool IsInGap(const uint32 Coordinate, const int32 StripDimension);
	static uint32 CalculateStripSize(const float ChunkDimension, const int32 GapCount);
};
