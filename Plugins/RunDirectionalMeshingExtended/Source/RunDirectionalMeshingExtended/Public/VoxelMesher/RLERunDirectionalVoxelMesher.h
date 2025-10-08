#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RUNDIRECTIONALMESHINGEXTENDED_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelChange>& VoxelChange) override;
	virtual void CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid) override;

	URLERunDirectionalVoxelMesher();

private:
	
	struct FMeshingEventInterval
	{
		// Voxel sequence (run) to be traversed
		// If null the end is a chunk dimension, not end of sequence
		const FRLEVoxel* CurrentVoxelRun = nullptr;
		
		// Index where event ends
		uint32 EventIndex = 0;
		
		// Index of an run in a voxel array
		uint32 VoxelRunIndex = 0;
	};

	// Type of faces the meshing interval should generate
	// Index to IntervalFaces array
	enum EMeshingTypeIndex {
		FullCulledFaceInterval = 0,
		FrontTopFaceInterval = 1,
		BackFaceInterval = 2,
		TopFaceInterval = 3,
		BottomFaceInterval = 4,
		FrontFaceInterval = 5,
		BackBottomFaceInterval = 6,
		EmptyFaceInterval = 7,
	};

	constexpr static int EventIndexCount = 4;
	enum EMeshingEventIndex
	{
		LeadingInterval = 0,
		FollowingXInterval = 1,
		FollowingZInterval = 2,
		EditEvent = 3
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
		
		TSharedPtr<TArray<FRLEVoxel>> NewVoxelGrid;
		TSharedPtr<TArray<FRLEVoxel>> VoxelGrid;

		// Current event index made of all meshing events that were already processed/traversed.
		int CurrentMeshingEventIndex = 0;

		// After reaching closest end, updates it and sets next voxel interval to next
		// End is equivalent to event in Discrete Event Simulation 
		FMeshingEventInterval MeshingEvents[EventIndexCount];
		EMeshingTypeIndex CurrentMeshingType;
		uint32 NextMeshingEventIndex = 0;

		TArray<FVoxelChange>* VoxelChanges = nullptr;
		bool EditEnabled = false;
	};

	static void CreateFace(const FMesherVariables& MeshVars,
	                       const FStaticMergeData& StaticData,
	                       const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                       const int YEnd);
	
	void FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars);

	// return true when interval advanced
	static bool AdvanceMeshingEventInterval(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex, bool BorderCondition = false, bool LeadingValue = false);
	
	static void CreateSideFace( TArray<TArray<FVoxelFace>>& SideFaceData,
											   const FStaticMergeData& StaticData,
											   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
											   const int YEnd);
	
	void AddBorderSample(const FIndexParams& IndexParams, const FIntVector IndexCoords, const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample, const int RunLenght) const;
	static void SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index);
	void BorderGeneration(FMesherVariables& MeshVars, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks) const;
	
	void GenerateBorder(TArray<FVoxelFace>& FaceContainer, TArray<FVoxelFace>& InverseFaceContainer,
		TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks,
		const FMeshingDirections& FaceTemplate, const FMeshingDirections& InverseFaceTemplate, int X, int Y) const;
};
