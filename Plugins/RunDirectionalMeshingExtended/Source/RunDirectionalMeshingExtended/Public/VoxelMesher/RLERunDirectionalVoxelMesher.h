#pragma once
#include <vector>

#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "Voxel/RLEVoxel.h"
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

		// TODO: merge bools into enums
		// If true this interval simulates neighboring chunk
		bool IsOuterInterval = true;
		// If true and is OuterIntervals it will generate border mesh 
		bool ShowBorders = true;

		// Traversed Run + current run sequence
		int RunEnd = 0;
		
		// Index of an run in a voxel array
		int32 RunIndex = -1;
	};

	// Type of faces the meshing interval should generate
	// Index to IntervalFaces array
	enum EIntervalType {
		FullCulledFace = 0,
		FrontTopFace = 1,
		BackFace = 2,
		TopFace = 3,
		BottomFace = 4,
		FrontFace = 5,
		BackBottomFace = 6,
		EmptyFace = 7,
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
		FIntVector Offset;
		EIntervalEndIndex IntervalEndIndex;
	};
	
	TStaticArray<TArray<FIntervalFace>, 8> IntervalFaces;

	struct FMeshingInterval
	{
		// Current voxel sequence that was already traversed.
		int TraversedVoxelSequence = 0;

		// After reaching closest end, updates it and sets next voxel interval to next
		// End is equivalent to event in Discrete Event Simulation 
		FIntervalEnd NextIntervalEnds[3];

		// Current Y coordinate
		int Y = 0;
	};
	
	struct FIndexParams
	{
		TSharedPtr<TArray<FRLEVoxel>> NewVoxelGrid;
		TSharedPtr<TArray<FRLEVoxel>> VoxelGrid;
		
		FMeshingInterval CurrentInterval; 

		FVoxel EditVoxel;
		FVoxel ReplacedVoxel = FVoxel();
		int32 EditAreaIndex = 0;

		FVoxelChange* VoxelChange = nullptr; 
	};
	
	void CreateFace(const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable,  const FStaticMergeData& StaticData,
										const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel, const int YEnd) const;

	static bool CalculateStartRunEditIndex(FIndexParams& IndexParams, int RunEnd);

	static bool CalculateBorderRunEditIndex(FIndexParams& IndexParams);

	static void FirstRunEditIndex(FIndexParams& IndexParams);
	static void CalculateMidRun(const int MidRunLenght, const int EndRunLength, FIndexParams& IndexParams);
	void FaceGeneration(FIndexParams& IndexParams, const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable) const;
};
