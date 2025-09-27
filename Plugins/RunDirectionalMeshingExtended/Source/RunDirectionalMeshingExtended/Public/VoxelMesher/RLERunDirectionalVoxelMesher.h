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
	virtual void GenerateMesh(FMesherVariables& MeshVars, FVoxelChange* VoxelChange) override;
	virtual void CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid) override;

	URLERunDirectionalVoxelMesher();

private:
	struct FVoxelIndexParams
	{
		bool IsBorder;
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		int32 CurrentVoxelIndex;
		FRLEVoxel CurrentVoxel;
		EFaceDirection FaceDirection;
	};
	
	struct FIntervalEnd
	{
		// Voxel sequence (run) to be traversed
		// If null the end is a chunk dimension, not end of sequence
		const FRLEVoxel* CurrentRun = nullptr;
		
		// Traversed Run + current run sequence
		int RunEnd = 0;
		
		// Index of an run in a voxel array
		uint32 RunIndex = 0;
	};

	// Type of faces the meshing interval should generate
	// Index to IntervalFaces array
	enum EIntervalType {
		FullCulledFaceInterval = 0,
		FrontTopFaceInterval = 1,
		BackFaceInterval = 2,
		TopFaceInterval = 3,
		BottomFaceInterval = 4,
		FrontFaceInterval = 5,
		BackBottomFaceInterval = 6,
		EmptyFaceInterval = 7,
	};

	enum EIntervalEndIndex
	{
		Leading = 0,
		FollowingX = 1,
		FollowingZ = 2
	};

	struct FIntervalFace
	{
		const FStaticMergeData FaceData;
		EIntervalEndIndex IntervalEndIndex;
	};
	
	// 8 = interval combinations
	TStaticArray<TArray<FIntervalFace>, 8> IntervalFaces;

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

		// Current voxel sequence that was already traversed.
		int TraversedVoxelSequence = 0;

		// After reaching closest end, updates it and sets next voxel interval to next
		// End is equivalent to event in Discrete Event Simulation 
		FIntervalEnd NextIntervalEnds[3];
		uint8 IntervalFlag = 0;
		int MinValue = 0;
		
		FVoxel EditVoxel;
		FVoxel ReplacedVoxel = FVoxel();
		int32 EditAreaIndex = 0;

		FVoxelChange* VoxelChange = nullptr; 
	};
	
	void CreateFace(const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable,
											   const FStaticMergeData& StaticData,
											   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
											   const int YEnd, FVoxelFace* PreviousFaces) const;

	static bool CalculateStartRunEditIndex(FIndexParams& IndexParams, int RunEnd);
	static bool CalculateBorderRunEditIndex(FIndexParams& IndexParams);
	static void FirstRunEditIndex(FIndexParams& IndexParams);
	static void CalculateMidRun(const int MidRunLenght, const int EndRunLength, FIndexParams& IndexParams);
	
	void FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable);

	// return true when interval advanced
	static bool AdvanceInterval(FIndexParams& IndexParams, const EIntervalEndIndex IntervalFlagIndex, bool BorderCondition, bool LeadingValue);
	void AddBorderSample(const FIndexParams& IndexParams, const FIntVector IndexCoords, const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample, const int RunLenght) const;
	static void SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index);
	void BorderGeneration(FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks);
	
	void GenerateBorder(
		TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks, const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces,
		const TSharedPtr<TArray<FVoxelFace>>& ReversedChunkFaces,
		const FMeshingDirections& FaceTemplate, int X, int Y);
};
