#pragma once
#include "BorderVisualizationOption.h"
#include "VoxelMesher/MeshEventPlanner/BorderVirtualMeshEventPlanner.h"

class UVirtualChunk;

struct FBorderParams
{
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, VOXEL_FACE_COUNT> SideMeshers;
	bool CanGenerate(EFaceDirection Direction, FIntVector BorderVoxelPosition, uint32 VoxelLine,
                     	const FVoxel& CurrentVoxel);
	
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
private:
	TStaticArray<FBorderVirtualMeshEventPlanner, VOXEL_FACE_COUNT> BorderIndexParams;
};
