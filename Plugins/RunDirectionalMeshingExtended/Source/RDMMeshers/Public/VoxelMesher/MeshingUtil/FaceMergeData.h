#pragma once
#include "VirtualVoxelFace.h"
#include "Enum/FaceDirection.h"

struct FFaceMergeData
{
	const EFaceDirection FaceDirection;
	const EFaceDirection InverseFaceDirection;
	
	const TFunctionRef<bool(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)> RunDirectionFaceMerge;
	const TFunctionRef<bool(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)> GreedyMerge;
	const TFunctionRef<FVirtualVoxelFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)> FaceCreator;
	const TFunctionRef<bool(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)> MergeFailCondition;
};
