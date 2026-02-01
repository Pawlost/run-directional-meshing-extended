#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "VoxelMesher/BorderVoxelEventMesher.h"
#include "VirtualChunk.generated.h"

struct FBorderParams;
struct FRLEVoxelEdit;
struct FVoxelMeshContainer;
class UBaseVoxelData;
/**
  * @brief Base class for components that convert voxel models into a mesh.
  */
UCLASS(Abstract, Blueprintable)
class RDMMESHERS_API UVirtualChunk : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category ="Voxels")
	bool bEnableVoxelMeshing = true;

	void SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase);

	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
	                          TArray<FRLEVoxelEdit>& VoxelChanges) PURE_VIRTUAL(UVirtualChunk::GenerateMesh)

	virtual void CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid) PURE_VIRTUAL(UVirtualChunk::NewVoxelGrid);

	virtual FVoxel GetBorderVoxel(FBorderEventMesher& IndexParams, const FIntVector& BorderVoxelPosition)
	PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder, return FVoxel(););

protected:
	UPROPERTY()
	TObjectPtr<UBaseVoxelData> VoxelData;
};
