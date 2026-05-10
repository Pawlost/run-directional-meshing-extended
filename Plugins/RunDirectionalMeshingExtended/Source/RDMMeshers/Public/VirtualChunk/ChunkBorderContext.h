#pragma once
#include "Voxel/Voxel.h"
#include "VoxelMesher/BorderVoxelEventMesher.h"
#include "VoxelMesher/MeshingUtil/Enum/BorderVisualizationOption.h"
#include "VoxelMesher/MeshingUtil/Enum/FaceDirection.h"
#include "VirtualChunkBase.h"

struct FChunkBorderContext
{
	TStaticArray<TStrongObjectPtr<UVirtualChunkBase>, VOXEL_FACE_COUNT> SideMeshers;
	bool CanGenerateBorder(const EFaceDirection Direction, const FIntVector& BorderVoxelPosition,
	                       const FVoxel& CurrentVoxel);

	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;

private:
	TStaticArray<FBorderEventMesher, VOXEL_FACE_COUNT> BorderIndexParams;
};
