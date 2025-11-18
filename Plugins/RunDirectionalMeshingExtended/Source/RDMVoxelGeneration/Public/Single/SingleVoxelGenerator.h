#pragma once
#include "CoreMinimal.h"
#include "SingleVoxelGeneratorBase.h"
#include "SingleVoxelGenerator.generated.h"

UCLASS(ClassGroup=(VoxelGeneration), Blueprintable)
class RDMVOXELGENERATION_API USingleVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
		virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) override;
};
