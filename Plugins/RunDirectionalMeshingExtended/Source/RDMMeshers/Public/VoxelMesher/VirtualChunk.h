#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "MeshEventPlanner/BorderVirtualMeshEventPlanner.h"
#include "MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "MeshingUtils/RLEVoxelEdit.h"
#include "VirtualChunk.generated.h"

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
	                          TArray<FRLEVoxelEdit>& VoxelChanges,
	                          EBorderVisualizationOption BorderVisualization) PURE_VIRTUAL(UVirtualChunk::GenerateMesh)

	virtual void CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid) PURE_VIRTUAL(UVirtualChunk::NewVoxelGrid);

	virtual FVoxel GetBorderVoxel(FBorderVirtualMeshEventPlanner& IndexParams, const FIntVector& BorderVoxelPosition)
	PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder, return FVoxel(););

protected:

	UPROPERTY()
	TObjectPtr<UBaseVoxelData> VoxelData;
};
