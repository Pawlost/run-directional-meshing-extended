#pragma once
#include "CoreMinimal.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "NoiseVoxelGridGenerator.generated.h"

struct FNoiseSurfaceGenerator;

UCLASS(ClassGroup=(VoxelGeneration), Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API UNoiseVoxelGridGenerator : public UVoxelGeneratorBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Voxels")
	TObjectPtr<UDataTable> VoxelTypeTable;

	virtual void GenerateVoxels(FChunk& Chunk) override;

	virtual double GetHighestElevationAtLocation(const FVector& Location) override;
	virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const int32& VoxelId) const override;
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "PrivateGenerators")
	TArray<FNoiseSurfaceGenerator> SurfaceGenerators;

private:
	static double ComputeSurfaceGradient(float PosX, float PosY, const TObjectPtr<UFastNoiseWrapper>& Generator,
	                                     double Elevation, double DistanceFromSurfaceLevel);
	bool IsChunkPositionOutOfBounds(double MinZPosition, double MaxZPosition) const;
};
