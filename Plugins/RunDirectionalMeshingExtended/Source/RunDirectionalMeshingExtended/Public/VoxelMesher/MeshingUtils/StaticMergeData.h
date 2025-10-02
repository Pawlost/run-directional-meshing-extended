#pragma once
#include "FaceDirection.h"
#include "VoxelFace.h"

struct FStaticMergeData
{
	const EFaceDirection FaceDirection;
	const EFaceDirection InverseFaceDirection;
	bool IsInverseDirection;
	
	const TFunctionRef<bool(FVoxelFace& PrevFace, const FVoxelFace& NewFace)> RunDirectionFaceMerge;
	const TFunctionRef<bool(FVoxelFace& PrevFace, const FVoxelFace& NewFace)> GreedyMerge;
	const TFunctionRef<FVoxelFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)> FaceCreator;
	const TFunctionRef<FIntVector(int X, int Y, int ChunkDimension)> BorderLocation;
	const TFunctionRef<bool(const FVoxelFace& Face, const FVoxelFace& NewFace)> RowBorderCondition;
	const TFunctionRef<int(FIntVector Location)> GetMainDirection;

	static FStaticMergeData FrontFaceData;
	static FStaticMergeData BackFaceData;
	static FStaticMergeData LeftFaceData;
	static FStaticMergeData RightFaceData;
	static FStaticMergeData TopFaceData;
	static FStaticMergeData BottomFaceData;
};