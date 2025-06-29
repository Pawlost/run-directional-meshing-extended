#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Voxel/Generator/Single/SingleVoxelGeneratorBase.h"
#include "RandomVoxelGenerator.generated.h"

UCLASS()
class DATASET_API URandomVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()
	
public:
	virtual void GenerateVoxels(FChunk& Chunk) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxel")
	int32 Seed = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax="1"), Category = "Voxel")
	float Threshold = 0.5f;
private:
	FRandomStream RandStream;
};
