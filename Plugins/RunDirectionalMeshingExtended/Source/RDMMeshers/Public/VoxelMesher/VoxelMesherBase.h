#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "MeshingUtils/MeshingDirections.h"
#include "MeshingUtils/ProcMeshSectionVars.h"
#include "MeshingUtils/RLEVoxelEdit.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/BorderChunk.h"
#include "VoxelModel/RLEVoxelGrid.h"
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
	void SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase);

	struct FBorderSamples
	{
		TStaticArray<TSharedPtr<TArray<FRLEVoxel>>, CHUNK_FACE_COUNT> BorderSamples;
		
		FBorderSamples(uint32 VoxelPlaneSize)
		{
			for (int i = 0; i < CHUNK_FACE_COUNT; i++)
			{
				BorderSamples[i] = MakeShared<TArray<FRLEVoxel>>();
				BorderSamples[i]->SetNum(VoxelPlaneSize);
			}
		}
		
		void AddBorderSample(const uint32 Index, const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample,
													const int RunLenght, bool CanSample)
		{
			const auto BorderSample = BorderSamples[FaceDirection];
			if (CanSample)
			{
				auto& BorderVoxel = (*BorderSample)[Index];
				BorderVoxel = VoxelSample;
				BorderVoxel.RunLenght = RunLenght;
			}
		}
	};
	
	virtual void GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
			TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
			TMap<int32, uint32>& LocalVoxelTable,
			TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
			TArray<FRLEVoxelEdit>& VoxelChanges,
			FBorderSamples& BorderSamples) PURE_VIRTUAL(UMesherBase::GenerateMesh)

	virtual TStrongObjectPtr<UVoxelModel> CompressVoxelModel(TArray<FVoxel>& VoxelGrid);

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
	
	virtual void SampleLeftChunkBorder(FBorderSamples& SampledBorderChunks, TSharedPtr<TArray<FRLEVoxel>> VoxelGrid) PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	virtual void SampleRightChunkBorder() PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	virtual void SampleTopChunkBorder() PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	virtual void SampleBottomChunkBorder() PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	virtual void SampleFrontChunkBorder() PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	virtual void SampleBackChunkBorder() PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder);
	
	
	virtual void BorderGeneration(const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
							TMap<int32, uint32>& BorderLocalVoxelTable, 
						   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
							TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples, 
							EFaceDirection FaceDirection)
							PURE_VIRTUAL(UVoxelMesherBase::GenerateBorder);
							
							
	void PreallocateArrays(TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
						   TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
						   TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData) const;
	
protected:
	struct FVoxelParams
	{
		FVoxel CurrentVoxel;
		FIntVector FacePosition;
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

	void ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray, TMap<int32, uint32>& LocalVoxelTable,
	                           const FVirtualVoxelFace& Face,
	                           int FaceIndex) const;

	void DirectionalGreedyMerge(TArray<FProcMeshSectionVars>& QuadMeshSectionArray,
	                            TMap<int32, uint32>& LocalVoxelTable,
	                            const FStaticMergeData& MergeData,
	                            TArray<FVirtualVoxelFace>& FaceContainer) const;

	static void AddFace(const FStaticMergeData& FaceMeshingData, const FVirtualVoxelFace& NewFace,
	                    TArray<FVirtualVoxelFace>& ChunkFaces);
	
	UPROPERTY()
	TObjectPtr<UBaseVoxelData> VoxelGenerator;
};
