#pragma once

#include "CoreMinimal.h"
#include "Single/SingleVoxelGeneratorBase.h"
#include "UObject/Object.h"
#include "ChessboardVoxelGenerator.generated.h"

UCLASS()
class DATASET_API UChessboardVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel)  override;

};
