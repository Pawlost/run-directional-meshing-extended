#pragma once
#include "VirtualChunk/ChunkBorderContext.h"
#include "VirtualChunk/VirtualChunkBase.h"

bool FChunkBorderContext::CanGenerateBorder(const EFaceDirection Direction, const FIntVector& BorderVoxelPosition,
                                            const FVoxel& CurrentVoxel)
{
	if (BorderVisualization == EBorderVisualizationOption::All)
	{
		return true;
	}

	if (SideMeshers[Direction] != nullptr)
	{
		const auto& BorderVoxel = SideMeshers[Direction]->GetBorderVoxel(
			BorderIndexParams[Direction], BorderVoxelPosition);
		return BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.IsTransparent());
	}

	return BorderVisualization == EBorderVisualizationOption::OnlyOuterBorders;
}
