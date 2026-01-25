#pragma once
#include "VoxelMesher/MeshingUtils/BorderParams.h"
#include "VoxelMesher/VirtualChunk.h"

bool FBorderParams::CanGenerate(EFaceDirection Direction, FIntVector BorderVoxelPosition, uint32 VoxelLine,
	const FVoxel& CurrentVoxel)
{
	if (BorderVisualization == EBorderVisualizationOption::All)
	{
		return true;
	}

	if (SideMeshers[Direction] != nullptr)
	{
		switch (Direction)
		{
		case EFaceDirection::Front:
			BorderVoxelPosition.X = 0;
			break;

		case EFaceDirection::Back:
			BorderVoxelPosition.X = VoxelLine - 1;
			break;

		case EFaceDirection::Top:
			BorderVoxelPosition.Z = 0;
			break;

		case EFaceDirection::Bottom:
			BorderVoxelPosition.Z = VoxelLine - 1;
			break;
		
		case EFaceDirection::Right:
			BorderVoxelPosition.Y = 0;
			break;

		case EFaceDirection::Left:
			BorderVoxelPosition.Y = VoxelLine - 1;
			break;
		}
	
		const auto& BorderVoxel = SideMeshers[Direction]->GetBorderVoxel(BorderIndexParams[Direction], BorderVoxelPosition);
		return BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.IsTransparent());
	}
	
	return BorderVisualization == EBorderVisualizationOption::OnlyOuterBorders;
}
