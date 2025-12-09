#pragma once
#include "CoreMinimal.h"
#include "SingleVoxelGeneratorBase.h"
#include "SingleVoxelGenerator.generated.h"

UCLASS(ClassGroup=(VoxelGeneration), Blueprintable)
class RDMVOXELGENERATION_API USingleVoxelGenerator : public USingleVoxelGeneratorBase
{
	GENERATED_BODY()

public:
		virtual void GenerateVoxels(const FVirtualVoxelChunk& Chunk,TArray<FVoxel>& VoxelModel) override;
};
