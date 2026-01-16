#include "VoxelMesher/MeshEventPlanner/VoxelIndexCounter.h"

void FVoxelIndexCounter::UpdatePositionFromIndex()
{
	CurrentVoxelPosition.X = CurrentVoxelIndex / (VoxelPlane);
	const int PosX = CurrentVoxelPosition.X * VoxelPlane;
	CurrentVoxelPosition.Z = (CurrentVoxelIndex - PosX) / VoxelLine;
	const int PosZ = CurrentVoxelPosition.Z * VoxelLine;
	CurrentVoxelPosition.Y = CurrentVoxelIndex - PosX - PosZ;
}

void FVoxelIndexCounter::UpdateIndexFromPosition()
{
	CurrentVoxelIndex = CurrentVoxelPosition.Y + (CurrentVoxelPosition.Z * VoxelLine) + (CurrentVoxelPosition.X * VoxelPlane);
}
