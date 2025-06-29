#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Voxel/Generator/Single/SingleVoxelGeneratorBase.h"
#include "ComplexShapeVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UComplexShapeVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(FChunk& Chunk) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax="10"), Category = "Voxel")
	float Omega = 0.5f;
private:
	float ShapeSinus(int Coordinate) const;
};
