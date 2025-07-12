#pragma once
#include "FaceDirection.h"
#include "VoxelFace.h"

struct FStaticMergeData
{
	const EFaceDirection FaceSide;
	const TFunctionRef<bool(FVoxelFace& PrevFace, const FVoxelFace& NewFace)> RunDirectionFaceMerge;
	const TFunctionRef<FVoxelFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)> FaceCreator;
	
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
	const TFunctionRef<bool(const FVoxelFace& Face, const FVoxelFace& NewFace, const int Coord2)> RowBorderCondition;
	const TFunctionRef<bool(const FVoxelFace& Face, const int Coord1, const int Coord2)> NextRowCondition;
	
	static FStaticGreedyMergeData FrontFace;
	static FStaticGreedyMergeData BackFace;
	static FStaticGreedyMergeData LeftFace;
	static FStaticGreedyMergeData RightFace;
	static FStaticGreedyMergeData TopFace;
	static FStaticGreedyMergeData BottomFace;
};