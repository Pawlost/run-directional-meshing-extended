﻿#pragma once
#include "CoreMinimal.h"
#include "MeshingUtils/FaceDirection.h"
#include "MeshingUtils/MeshingDirections.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "VoxelMesherBase.generated.h"

struct FChunkParams;

struct FVoxelChange;
struct FMesherVariables;

/**
  * @brief Base class for components that convert voxel models into a mesh.
  */
UCLASS(Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGEXTENDED_API UVoxelMesherBase : public UActorComponent
{
	GENERATED_BODY()
	
public:
	void SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase);
	virtual void GenerateMesh(FMesherVariables& MesherVariables, FVoxelChange* VoxelChange) PURE_VIRTUAL(UMesherBase::GenerateMesh)
	virtual void CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid);

protected:
	FMeshingDirections FrontFaceTemplate = FMeshingDirections(FStaticMergeData::FrontFaceData);
	FMeshingDirections BackFaceTemplate = FMeshingDirections(FStaticMergeData::BackFaceData);
	FMeshingDirections RightFaceTemplate = FMeshingDirections(FStaticMergeData::RightFaceData);
	FMeshingDirections LeftFaceTemplate = FMeshingDirections(FStaticMergeData::LeftFaceData);
	FMeshingDirections TopFaceTemplate = FMeshingDirections(FStaticMergeData::TopFaceData);
	FMeshingDirections BottomFaceTemplate = FMeshingDirections(FStaticMergeData::BottomFaceData);
	
	struct FNormalsAndTangents
	{
		FVector Normal;
		FProcMeshTangent Tangent;
	};
     
	FORCEINLINE static bool IsMinBorder(const int X)
	{
		return X == 0;
	}
     
	FORCEINLINE bool IsMaxBorder(const int X) const
	{
		return X == VoxelGenerator->GetVoxelCountPerChunkDimension() - 1;
	}
     
	static const FNormalsAndTangents FaceNormalsAndTangents[CHUNK_FACE_COUNT];
 
	void UpdateAllFacesParams();
	void UpdateFaceParams(FMeshingDirections& Face, FIntVector ForwardVoxelIndexVector,
						  FIntVector PreviousVoxelIndexVector, FIntVector ChunkBorderIndexVector) const;

	static bool EmptyActor(const FMesherVariables& MeshVars);
	
	void PreallocateArrays(FMesherVariables& MeshVars) const;
	
	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;
};
