#pragma once
#include "FaceDirection.h"
#include "VoxelFace.h"

struct FStaticMergeData
{
	const EFaceDirection FaceDirection;
	const EFaceDirection InverseFaceDirection;
	bool IsInverseDirection;
	
	const TFunctionRef<bool(FVoxelFace& PrevFace, const FVoxelFace& NewFace)> RunDirectionFaceMerge;
	const TFunctionRef<FVoxelFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)> FaceCreator;
	const TFunctionRef<FIntVector(int X, int Y, int ChunkDimension)> BorderLocation;

	static FStaticMergeData FrontFaceData;
	static FStaticMergeData BackFaceData;
	static FStaticMergeData LeftFaceData;
	static FStaticMergeData RightFaceData;
	static FStaticMergeData TopFaceData;
	static FStaticMergeData BottomFaceData;
};

struct FStaticGreedyMergeData
{
	const EFaceDirection FaceSide;
	const TFunctionRef<bool(const FVoxelFace& Face, const FVoxelFace& NewFace)> RowBorderCondition;
	
	static FStaticGreedyMergeData FrontFace;
	static FStaticGreedyMergeData BackFace;
	static FStaticGreedyMergeData LeftFace;
	static FStaticGreedyMergeData RightFace;
	static FStaticGreedyMergeData TopFace;
	static FStaticGreedyMergeData BottomFace;
};