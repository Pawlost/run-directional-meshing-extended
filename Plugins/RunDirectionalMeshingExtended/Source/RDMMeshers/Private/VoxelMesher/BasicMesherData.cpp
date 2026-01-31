#include "VoxelMesher/MeshEventPlanner/BasicMesherData.h"

const TStaticArray<FIntVector, VOXEL_FACE_COUNT> FBasicMesherData::VoxelPositionOffsets = {
	FIntVector(1, 0, 0),
	FIntVector(0, 0, 0),
	FIntVector(0, 1, 0),
	FIntVector(0, 0, 0),
	FIntVector(0, 0, 1),
	FIntVector(0, 0, 0),
};

void FBasicMesherData::UpdatePositionFromIndex()
{
	VoxelPosition.X = VoxelIndex / (VoxelPlane);
	const int PosX = VoxelPosition.X * VoxelPlane;
	VoxelPosition.Z = (VoxelIndex - PosX) / VoxelLine;
	const int PosZ = VoxelPosition.Z * VoxelLine;
	VoxelPosition.Y = VoxelIndex - PosX - PosZ;
}

uint32 FBasicMesherData::CalculateIndexFromPosition(const FIntVector& Position) const
{
	return Position.Y + (Position.Z * VoxelLine) + (Position.X * VoxelPlane);
}

void FBasicMesherData::UpdateIndexFromPosition()
{
	VoxelIndex = CalculateIndexFromPosition(VoxelPosition);
}
