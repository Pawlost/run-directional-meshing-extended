#pragma once
#include "Voxel/Voxel.h"
#include "VoxelMesher/BorderVoxelEventMesher.h"
#include "VoxelMesher/MeshingUtil/Enum/BorderVisualizationOption.h"
#include "VoxelMesher/MeshingUtil/Enum/FaceDirection.h"

class UVirtualChunk;

struct FChunkBorderContext
{
	
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, VOXEL_FACE_COUNT> SideMeshers;
	bool CanGenerateBorder(const EFaceDirection Direction, const FIntVector& BorderVoxelPosition, 
		const FVoxel& CurrentVoxel);
	
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
private:
	TStaticArray<FBorderEventMesher, VOXEL_FACE_COUNT> BorderIndexParams;
};
