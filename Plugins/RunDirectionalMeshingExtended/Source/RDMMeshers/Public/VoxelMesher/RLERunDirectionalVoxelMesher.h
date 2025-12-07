#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
		TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
		TMap<int32, uint32>& LocalVoxelTable,
		TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
		TArray<FRLEVoxelEdit>& VoxelChanges,
		FBorderSamples& BorderSamples) override;

	virtual TStrongObjectPtr<UVoxelModel> CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual void SampleLeftChunkBorder(FBorderSamples& SampledBorderChunks, TSharedPtr<TArray<FRLEVoxel>> VoxelGrid) override;
	
	void AddLeftBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddRightBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddTopBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddBottomBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddFrontBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddBackBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	
	
	virtual void BorderGeneration(const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
							TMap<int32, uint32>& BorderLocalVoxelTable, 
						   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
							TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples, 
							EFaceDirection FaceDirection) override;


PRIVATE_MODIFIER:
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

	// 8 = interval combinations/types
	TStaticArray<TArray<FRLEMeshingData>, 8> MeshingData;

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
		FBorderSamples* SampledBorderChunks;
		TSharedPtr<TArray<FRLEVoxel>> VoxelGrid;

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

		void TryUpdateNextMeshingEvent(const uint32 EventIndex)
		{
			NextMeshingEventIndex = FMath::Min(EventIndex, NextMeshingEventIndex);
		}
		
		void AddBorderSample(const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample,
													const int RunLenght, bool CanSample)
		{
			SampledBorderChunks->AddBorderSample(CurrentMeshingEventIndex, FaceDirection, VoxelSample, RunLenght, CanSample);
		}
	};

	static void CreateFace(const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
											   const FStaticMergeData& StaticData,
											   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
											   const int YEnd, const bool CanGenerate);

	void FaceGeneration(FIndexParams& IndexParams);
	void AdvanceAllMeshingEvents(FIndexParams& IndexParams, int X, int Y, int Z);

	// return true when interval advanced
	static bool AdvanceMeshingEvent(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex);

	static void CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
	                           const FStaticMergeData& StaticData,
	                           const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                           const int YEnd);
	
	static void SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index);

	void AddBorderFace(TArray<FVirtualVoxelFace>& FaceContainer,
												   TArray<FVirtualVoxelFace>& InverseFaceContainer,
												   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
													TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples,
												   const FMeshingDirections& FaceTemplate,
												   const FMeshingDirections& InverseFaceTemplate, int X, int Y) const;

	void CreateVirtualVoxelFacesInLShape(FIndexParams& IndexParams, int X, int Y, int Z);
	void InitializeEdit(FIndexParams& IndexParams);
	void EditVoxelGrid(FIndexParams& IndexParams);

	void AdvanceEditInterval(FIndexParams& IndexParams) const;
};
