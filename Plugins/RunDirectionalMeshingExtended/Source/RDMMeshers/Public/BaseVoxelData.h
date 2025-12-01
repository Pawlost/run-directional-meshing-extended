#pragma once
#include "CoreMinimal.h"
#include "Tests/AutomationTestAccessModifiers.h"
#include "BaseVoxelData.generated.h"
/**
 * Base for components used to fill voxel models with voxels.
 */
UCLASS(ClassGroup=(VoxelGeneration), Blueprintable)
class RDMMESHERS_API UBaseVoxelData : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Voxels",
		meta=(ToolTip=
			"The number of voxels in a single dimension (X, Y, or Z) of the chunk. This value represents the count of voxels along one axis of the chunk, not the total number of voxels in the entire chunk."
		, MinClamp="0"))
	int64 VoxelCountPerChunkDimension = 32;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category ="Voxels",
		meta=(ToolTip="Size of a single voxel in world coordinates.", MinClamp="0"))
	double VoxelSize = 20;
	
	/**
	 * Calculate voxel index in chunk grid from grid coordinates.
	 */
	uint32 CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const;

	/**
	 * Calculate voxel index in chunk grid from vector grid coordinates.
	 */
	FORCEINLINE uint32 CalculateVoxelIndex(const FIntVector& VoxelPosition) const;

	FORCEINLINE double GetChunkSpacing() const;
	FORCEINLINE double GetVoxelSize() const;
	FORCEINLINE uint32 GetVoxelCountPerVoxelLine() const;
	FORCEINLINE uint32 GetVoxelCountPerVoxelPlane() const;
	FORCEINLINE uint32 GetVoxelCountPerChunk() const;

	UFUNCTION(BlueprintCallable)
	virtual double GetHighestElevationAtLocation(const FVector& Location);
	
protected:
	virtual void BeginPlay() override;
	
PRIVATE_MODIFIER:
	void CalculateVoxelData();
	double ChunkSpacing = 0.0, InternalVoxelSize = 0.0;
	int32 VoxelCountY = 0, VoxelCountYZ = 0, VoxelCountXYZ = 0;
	FCriticalSection Mutex;
};
