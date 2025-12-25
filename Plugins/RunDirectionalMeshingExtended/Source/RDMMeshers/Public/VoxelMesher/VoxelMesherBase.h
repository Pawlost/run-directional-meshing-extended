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
		
	UPROPERTY(EditAnywhere, Category ="Voxels")
	bool bEnableVoxelMeshing = true;

	UPROPERTY()
	TObjectPtr<UBaseVoxelData> VoxelData;
	
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
	
	struct FMeshingEvent
	{
		// Voxel sequence (run) to be traversed
		// If null the end is a chunk dimension, not end of sequence
		TSharedPtr<TArray<FRLEVoxel>> VoxelGridPtr;

		// Index where event ends
		uint32 LastEventIndex = 0;

		// Index of an run in a voxel array
		int32 VoxelRunIndex = 0;

		FORCEINLINE int GetEventIndex() const
		{
			return LastEventIndex + (*VoxelGridPtr)[VoxelRunIndex].RunLenght;
		}

		FORCEINLINE void AdvanceEvent()
		{
			LastEventIndex = LastEventIndex + GetCurrentVoxel().RunLenght;
			VoxelRunIndex++;
		}

		FORCEINLINE FRLEVoxel& GetCurrentVoxel() const
		{
			return (*VoxelGridPtr)[VoxelRunIndex];
		}
	};
	
	
	constexpr static int EventIndexCount = 5;

	enum EMeshingEventIndex
	{
		LeadingInterval = 0,
		FollowingXInterval = 1,
		FollowingZInterval = 2,
		EditEvent = 3,
		CopyEvent = 4
	};

	struct FRLEMeshingData
	{
		const FStaticMergeData FaceData;
		EMeshingEventIndex MeshingEventIndex;
	};

	/*
Front = 0,
Back = 1,
Right = 2,
Left = 3,
Bottom = 4,
Top = 5
*/
	struct FIndexParams
	{
		// Current event index made of all meshing events that were already processed/traversed.
		uint32 CurrentMeshingEventIndex = 0;

		// After reaching closest end, updates it and sets next voxel interval to next
		// End is equivalent to event in Discrete Event Simulation 
		
		FMeshingEvent MeshingEvents[EventIndexCount];
		uint32 NextMeshingEventIndex = 0;
		uint32 IndexSequenceBetweenEvents = 0;
		FRLEVoxel* PreviousVoxelRun = nullptr;

		uint32 ContinueEditIndex = 0;

		//TODO: rewrite pointer
		TArray<FRLEVoxelEdit>* VoxelEdits = nullptr;
		bool EditEnabled = false;
		FIntVector InitialPosition = FIntVector(0, 0, 0);
		
		TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces;
		
		void TryUpdateNextMeshingEvent(const uint32 EventIndex)
		{
			NextMeshingEventIndex = FMath::Min(EventIndex, NextMeshingEventIndex);
		}
	};
	
	
	virtual void GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
							  TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, 6>& VirtualFaces,
							  TMap<int32, uint32>& LocalVoxelTable,
							  TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
							  TArray<FRLEVoxelEdit>& VoxelChanges,
								TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
							  TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers, FBorderSamples& BorderSamples) PURE_VIRTUAL(UMesherBase::GenerateMesh)

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) PURE_VIRTUAL(UMesherBase::GenerateMesh);

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
	
	// TODO: use simplier struct than index params
	virtual bool IsBorderVoxelEmpty(FIndexParams& IndexParams, int X, int Y, int Z) PURE_VIRTUAL(UVoxelMesherBase::SampleLeftBorder, return false; );
	
	virtual void BorderGeneration(const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
							TMap<int32, uint32>& BorderLocalVoxelTable, 
						   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
							TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples, 
							EFaceDirection FaceDirection)
							PURE_VIRTUAL(UVoxelMesherBase::GenerateBorder);
							
							
	void PreallocateArrays(TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
							TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
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
		return X == VoxelData->GetVoxelCountPerVoxelLine() - 1;
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
	
};
