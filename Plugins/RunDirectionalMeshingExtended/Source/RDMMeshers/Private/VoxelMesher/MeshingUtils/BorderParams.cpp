#pragma once
#include "VoxelMesher/MeshingUtils/BorderParams.h"
#include "VoxelMesher/VirtualChunk.h"

bool FBorderParams::CanGenerateBorder(EFaceDirection Direction, FIntVector BorderVoxelPosition, 
	const FVoxel& CurrentVoxel)
{
	if (BorderVisualization == EBorderVisualizationOption::All)
	{
		return true;
	}

	if (SideMeshers[Direction] != nullptr)
	{
		const auto& BorderVoxel = SideMeshers[Direction]->GetBorderVoxel(BorderIndexParams[Direction], BorderVoxelPosition);
		return BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.IsTransparent());
	}
	
	return BorderVisualization == EBorderVisualizationOption::OnlyOuterBorders;
}