#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Single/SingleVoxelGeneratorBase.h"
#include "SphereVoxelGenerator.generated.h"

UCLASS()
class DATASET_API USphereVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()
	
public:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Voxel")
	float SphereRadius = 10.0;
};
