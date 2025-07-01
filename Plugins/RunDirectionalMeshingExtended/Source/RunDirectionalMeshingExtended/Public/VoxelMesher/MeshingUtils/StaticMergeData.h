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
