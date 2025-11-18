#pragma once

#include "CoreMinimal.h"
#include "Single/SingleVoxelGeneratorBase.h"
#include "UObject/Object.h"
#include "ComplexShapeVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UComplexShapeVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)  override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax="10"), Category = "Voxel")
	float Omega = 0.5f;
private:
	float ShapeSinus(int Coordinate) const;

};
