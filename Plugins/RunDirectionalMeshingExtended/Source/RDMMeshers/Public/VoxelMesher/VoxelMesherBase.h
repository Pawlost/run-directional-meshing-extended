#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "MeshingUtils/MeshingDirections.h"
#include "MeshingUtils/ProcMeshSectionVars.h"
#include "MeshingUtils/RLEVoxelEdit.h"
#include "MeshingUtils/VirtualMeshEventPlanner.h"
#include "MeshingUtils/VirtualVoxelFace.h"
#include "Voxel/RLEVoxel.h"
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
	
	virtual void GenerateMesh(TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, 6>& VirtualFaces,
								FVoxelMeshContainer& MeshContainer,
							  TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
							  TArray<FRLEVoxelEdit>& VoxelChanges,
								TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
							  TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers, bool ShowBorders) PURE_VIRTUAL(UMesherBase::GenerateMesh)

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) PURE_VIRTUAL(UMesherBase::GenerateMesh);
	
	// TODO: use simplier struct than index params
	virtual FVoxel GetBorderVoxel(FVirtualMeshEventPlanner& IndexParams, FIntVector VoxelPosition) PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder, return FVoxel(); );
	
	virtual void BorderGeneration(const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
							TMap<int32, uint32>& BorderLocalVoxelTable, 
						   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
							TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples, 
							EFaceDirection FaceDirection)
							PURE_VIRTUAL(UVoxelMesherBase::GenerateBorder);
							
							
	void PreallocateArrays(TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
							TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces) const;
	
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
