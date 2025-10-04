#pragma once
#include "CoreMinimal.h"
#include "MeshingUtils/FaceDirection.h"
#include "MeshingUtils/MeshingDirections.h"
#include "MeshingUtils/ProcMeshSectionVars.h"
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
	
	struct FVoxelParams
	{
		FVoxel CurrentVoxel;
		FIntVector FacePosition;
	};
	
	/*
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Bottom = 4,
	Top = 5
	*/
	FMeshingDirections FaceTemplates[CHUNK_FACE_COUNT] = {
		FMeshingDirections(FStaticMergeData::FrontFaceData), FMeshingDirections(FStaticMergeData::BackFaceData),
		FMeshingDirections(FStaticMergeData::RightFaceData), FMeshingDirections(FStaticMergeData::LeftFaceData),
		FMeshingDirections(FStaticMergeData::BottomFaceData), FMeshingDirections(FStaticMergeData::TopFaceData)
	};
	
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
		return X == VoxelGenerator->GetVoxelCountPerVoxelLine() - 1;
	}
     
	static const FNormalsAndTangents FaceNormalsAndTangents[CHUNK_FACE_COUNT];
 
	void UpdateAllFacesParams();
	void UpdateFaceParams(FMeshingDirections& Face, FIntVector ForwardVoxelIndexVector,
						  FIntVector ChunkBorderIndexVector, FIntVector PreviousVoxelIndexVector) const;

	static bool EmptyActor(const FMesherVariables& MeshVars);
	
	void PreallocateArrays(FMesherVariables& MeshVars) const;
	
	void GenerateProcMesh(const FMesherVariables& MeshVars, TMap<uint32, uint32> LocalVoxelTable) const;
	
	void ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray, const FVoxelFace& Face,
								  TMap<uint32, uint32>& LocalVoxelTable, int FaceIndex) const;
	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;

	void DirectionalGreedyMerge(const FMesherVariables& MeshVars,
														TMap<uint32, uint32>& LocalVoxelTable,
														const FStaticMergeData& MergeData,
														TArray<FVoxelFace>& FaceContainer) const;

	static void AddFace(const FStaticMergeData& FaceMeshingData, const FVoxelFace& NewFace, TArray<FVoxelFace>& ChunkFaces);
};
