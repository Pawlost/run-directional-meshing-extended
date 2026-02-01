#pragma once
#include "BorderVisualizationOption.h"
#include "FaceDirection.h"
#include "Voxel/Voxel.h"
#include "VoxelMesher/BorderVoxelEventMesher.h"

class UVirtualChunk;

struct FBorderParams
{
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, VOXEL_FACE_COUNT> SideMeshers;
	bool CanGenerateBorder(const EFaceDirection Direction, const FIntVector& BorderVoxelPosition, 
		const FVoxel& CurrentVoxel);
	
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
private:
	TStaticArray<FBorderEventMesher, VOXEL_FACE_COUNT> BorderIndexParams;
};
