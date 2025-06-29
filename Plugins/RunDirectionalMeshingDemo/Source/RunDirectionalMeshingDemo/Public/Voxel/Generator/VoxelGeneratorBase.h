#pragma once
#include "CoreMinimal.h"
#include "Chunk/Chunk.h"
#include "Voxel/Voxel.h"
#include "Voxel/VoxelType.h"
#include "VoxelGeneratorBase.generated.h"

struct FVoxelChange;
struct FMesherVariables;
class UVoxelMesherBase;

/**
 * Base for components used to fill voxel models with voxels.
 */
UCLASS(ClassGroup=(VoxelGeneration), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API UVoxelGeneratorBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowAbstract="false", BlueprintBaseOnly), NoClear,
		Category="Voxels")
	TSubclassOf<UVoxelMesherBase> VoxelMesherBlueprint = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category="Voxels",
		meta=(ToolTip=
			"The number of voxels in a single dimension (X, Y, or Z) of the chunk. This value represents the count of voxels along one axis of the chunk, not the total number of voxels in the entire chunk."
		, MinClamp="0"))
	int64 VoxelCountPerChunkDimension = 32;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category ="Voxels",
		meta=(ToolTip="Size of a single voxel in world coordinates.", MinClamp="0"))
	double VoxelSize = 20;


	UPROPERTY(EditAnywhere, Category ="Voxels")
	bool bEnableVoxelMeshing = true;
	
	void ChangeKnownVoxelAtIndex(TArray<FVoxel>& VoxelGrid, TMap<int32, uint32>& VoxelTable, const uint32& Index,
												  const FVoxel& Voxel);
	
	/**
	 * Calculate voxel index in chunk grid from grid coordinates.
	 */
	uint32 CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const;

	/**
	 * Calculate voxel index in chunk grid from vector grid coordinates.
	 */
	uint32 CalculateVoxelIndex(const FIntVector& VoxelPosition) const;

	double GetChunkAxisSize() const;
	double GetVoxelSize() const;
	uint32 GetVoxelCountPerChunkDimension() const;
	uint32 GetVoxelCountPerChunkLayer() const;
	uint32 GetVoxelCountPerChunk() const;
	void GenerateMesh(FMesherVariables& MesherVariables, FVoxelChange* VoxelChange = nullptr) const;

	UFUNCTION(BlueprintCallable)
	virtual double GetHighestElevationAtLocation(const FVector& Location);

	// Abstract functions
	virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const int32& VoxelId) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelType, return TTuple<FName, FVoxelType>();)

	FORCEINLINE virtual TTuple<FName, FVoxelType> GetVoxelTypeById(const FVoxel& Voxel){
		return GetVoxelTypeById(Voxel.VoxelId);
	}
	
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelByName, return FVoxel();)

	virtual void GenerateVoxels(FChunk& Chunk) PURE_VIRTUAL(UVoxelGeneratorBase::GenerateVoxels)
	static void RemoveVoxelFromChunkTable(TMap<int32, uint32>& VoxelTable, const FVoxel& Voxel);
	
protected:
	UPROPERTY()
	TObjectPtr<UVoxelMesherBase> VoxelMesher;
	virtual void BeginPlay() override;

private:
	double ChunkSize = 0.0, InternalVoxelSize = 0.0;
	int32 VoxelCountY = 0, VoxelCountYZ = 0, VoxelCountXYZ = 0;
	FCriticalSection Mutex;
};
