#include "VoxelMesher/MeshEventPlanner/VoxelIndexCounter.h"

void FVoxelIndexCounter::UpdatePositionFromIndex()
{
	VoxelPosition.X = VoxelIndex / (VoxelPlane);
	const int PosX = VoxelPosition.X * VoxelPlane;
	VoxelPosition.Z = (VoxelIndex - PosX) / VoxelLine;
	const int PosZ = VoxelPosition.Z * VoxelLine;
	VoxelPosition.Y = VoxelIndex - PosX - PosZ;
}

uint32 FVoxelIndexCounter::CalculateIndexFromPosition(const FIntVector& Position) const
{
	return Position.Y + (Position.Z * VoxelLine) + (Position.X * VoxelPlane);
}

void FVoxelIndexCounter::UpdateIndexFromPosition()
{
	VoxelIndex = CalculateIndexFromPosition(VoxelPosition);
}
