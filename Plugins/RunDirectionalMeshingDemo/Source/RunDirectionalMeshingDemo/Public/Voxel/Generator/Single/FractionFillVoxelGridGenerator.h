#pragma once
#include "CoreMinimal.h"
#include "SingleVoxelGeneratorBase.h"
#include "FractionFillVoxelGridGenerator.generated.h"

UCLASS(ClassGroup=(VoxelGeneration), Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API UFractionFillVoxelGridGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 XFraction = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 YFraction = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = "1"), Category = "Voxel grid size")
	int32 ZFraction = 1;

	virtual void GenerateVoxels(FChunk& Chunk) override;
};
