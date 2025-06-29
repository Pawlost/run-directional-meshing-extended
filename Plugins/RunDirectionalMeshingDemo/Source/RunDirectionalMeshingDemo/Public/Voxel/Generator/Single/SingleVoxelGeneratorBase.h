#pragma once

#include "CoreMinimal.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "SingleVoxelGeneratorBase.generated.h"

UCLASS(ClassGroup=(VoxelGeneration), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API USingleVoxelGeneratorBase : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Voxels")
	FDataTableRowHandle VoxelTypeHandle;

	virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const int32& Voxel) const override;
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const override;
protected:
	virtual void BeginPlay() override;
	FVoxel GetSingleVoxel() const;
};
