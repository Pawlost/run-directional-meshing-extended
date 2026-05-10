#pragma once
#include "VoxelMesher/BasicVoxelMesher.h"

struct FBasicVoxelMesherDummy : public FBasicVoxelMesher
{
public:
	FBasicVoxelMesherDummy(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
		: FBasicVoxelMesher(BaseVoxelData)
	{
	}

	using FBasicVoxelMesher::CheckBorder;
	using FBasicVoxelMesher::CheckInnerVoxel;
	using FBasicVoxelMesher::GenerateVirtualFaces;
	using FBasicVoxelMesher::ConvertVirtualFacesToMesh;
	using FBasicVoxelMesher::VoxelPosition;
	using FBasicVoxelMesher::VoxelIndex;
	using FBasicVoxelMesher::UpdateIndexFromPosition;
	using FBasicVoxelMesher::VoxelGrid;
	using FBasicVoxelMesher::TempVoxels;
	using FBasicVoxelMesher::VoxelSideParams;
};
