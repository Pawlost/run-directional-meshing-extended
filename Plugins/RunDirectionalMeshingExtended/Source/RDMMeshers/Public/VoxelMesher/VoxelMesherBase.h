#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "MeshEventPlanner/BorderVirtualMeshEventPlanner.h"
#include "MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "MeshingUtils/MeshingDirections.h"
#include "MeshingUtils/RLEVoxelEdit.h"
#include "MeshingUtils/VoxelMeshContainer.h"
#include "VoxelMesherBase.generated.h"

class UBaseVoxelData;
/**
  * @brief Base class for components that convert voxel models into a mesh.
  */
UCLASS(Abstract, Blueprintable)
class RDMMESHERS_API UVoxelMesherBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category ="Voxels")
	bool bEnableVoxelMeshing = true;
	
	void SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase);
	
	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
	                          TArray<FRLEVoxelEdit>& VoxelChanges, bool ShowBorders) PURE_VIRTUAL(UMesherBase::GenerateMesh)

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) PURE_VIRTUAL(UMesherBase::GenerateMesh);
	
	virtual FVoxel GetBorderVoxel(FBorderVirtualMeshEventPlanner& IndexParams, const FIntVector& BorderVoxelPosition) PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder, return FVoxel(); );

protected:
	struct FVoxelParams
	{
		FVoxel CurrentVoxel;
		FIntVector FacePosition;
	};

	//void UpdateAllFacesParams();
	void UpdateFaceParams(FMeshingDirections& Face, FIntVector ForwardVoxelIndexVector,
	                      FIntVector ChunkBorderIndexVector, FIntVector PreviousVoxelIndexVector) const;

	
	UPROPERTY()
	TObjectPtr<UBaseVoxelData> VoxelData;
};
