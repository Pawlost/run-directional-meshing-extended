#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Single/SingleVoxelGeneratorBase.h"
#include "Voxel/Voxel.h"
#include "Chunk/Chunk.h"
#include "StripsVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UStripsVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()
	
public:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)  override;

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
