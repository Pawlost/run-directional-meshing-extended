﻿#pragma once
#include "FaceDirection.h"
#include "VirtualVoxelFace.h"

struct FStaticMergeData
{
	const EFaceDirection FaceDirection;
	const EFaceDirection InverseFaceDirection;
	bool IsInverseDirection;
	
	const TFunctionRef<bool(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)> RunDirectionFaceMerge;
	const TFunctionRef<bool(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)> GreedyMerge;
	const TFunctionRef<FVirtualVoxelFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)> FaceCreator;
	const TFunctionRef<void(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)> BorderLocation;
	const TFunctionRef<int(FIntVector Location)> GetMainDirection;
	
	const TFunctionRef<bool(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)> HeightCondition;
	const TFunctionRef<bool(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)> MergeFailCondition;

	static FStaticMergeData FrontFaceData;
	static FStaticMergeData BackFaceData;
	static FStaticMergeData LeftFaceData;
	static FStaticMergeData RightFaceData;
	static FStaticMergeData TopFaceData;
	static FStaticMergeData BottomFaceData;
};