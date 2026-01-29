#pragma once
#include "BorderVisualizationOption.h"
#include "VoxelMesher/MeshEventPlanner/BorderVirtualMeshEventPlanner.h"

class UVirtualChunk;

struct FBorderParams
{
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, VOXEL_FACE_COUNT> SideMeshers;
	bool CanGenerateBorder(EFaceDirection Direction, FIntVector BorderVoxelPosition, 
		const FVoxel& CurrentVoxel);
	
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
private:
	TStaticArray<FBorderVirtualMeshEventPlanner, VOXEL_FACE_COUNT> BorderIndexParams;
};
