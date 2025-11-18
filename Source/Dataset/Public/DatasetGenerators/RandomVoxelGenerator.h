#pragma once

#include "CoreMinimal.h"
#include "Single/SingleVoxelGeneratorBase.h"
#include "UObject/Object.h"
#include "RandomVoxelGenerator.generated.h"

UCLASS()
class DATASET_API URandomVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()
	
public:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)  override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxel")
	int32 Seed = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax="1"), Category = "Voxel")
	float Threshold = 0.5f;
private:
	FRandomStream RandStream;
};
