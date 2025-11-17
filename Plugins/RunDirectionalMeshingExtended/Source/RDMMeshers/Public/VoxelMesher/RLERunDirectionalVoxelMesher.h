#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "Voxel/RLEVoxel.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel, 
	                          TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces,
	                          TMap<int32, uint32> LocalVoxelTable,
	                          const TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	                          TArray<FVoxelEdit>& VoxelChange) override;
	
	virtual void CompressVoxelGrid(TStrongObjectPtr<UVoxelModel>& VoxelModel, TArray<FVoxel>& VoxelGrid) override;

private:
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
		TSharedPtr<TArray<FRLEVoxel>> SampledBorderChunks[CHUNK_FACE_COUNT];
		TSharedPtr<TArray<FRLEVoxel>> VoxelGrid;

		// Current event index made of all meshing events that were already processed/traversed.
		uint32 CurrentMeshingEventIndex = 0;

		// After reaching closest end, updates it and sets next voxel interval to next
		// End is equivalent to event in Discrete Event Simulation 
		FMeshingEvent MeshingEvents[EventIndexCount];
		uint32 NextMeshingEventIndex = 0;

		uint32 ContinueEditIndex = 0;

		TArray<FVoxelEdit>* VoxelChanges = nullptr;
		bool EditEnabled = false;

		void TryUpdateNextMeshingEvent(const uint32 EventIndex)
		{
			NextMeshingEventIndex = FMath::Min(EventIndex, NextMeshingEventIndex);
		}
	};

	static void URLERunDirectionalVoxelMesher::CreateFace(const TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces,
	                                                      const FStaticMergeData& StaticData,
	                                                      const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                                                      const int YEnd, const bool CanGenerate);

	void FaceGeneration(FIndexParams& IndexParams, const TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces);

	// return true when interval advanced
	static bool AdvanceMeshingEvent(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex);

	static void CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
	                           const FStaticMergeData& StaticData,
	                           const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                           const int YEnd);

	void AddBorderSample(const FIndexParams& IndexParams, const FIntVector IndexCoords,
	                     const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample, const int RunLenght) const;
	
	static void SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index);
	
	void BorderGeneration(FMesherVariables& MeshVars, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks) const;

	void GenerateBorder(TArray<FVirtualVoxelFace>& FaceContainer, TArray<FVirtualVoxelFace>& InverseFaceContainer,
	                    TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks,
	                    const FMeshingDirections& FaceTemplate, const FMeshingDirections& InverseFaceTemplate, int X,
	                    int Y) const;

	void AdvanceEditInterval(FIndexParams& IndexParams) const;
};
