#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Voxel/Generator/Single/SingleVoxelGeneratorBase.h"
#include "ChessboardVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UChessboardVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void GenerateVoxels(FChunk& Chunk) override;

};
