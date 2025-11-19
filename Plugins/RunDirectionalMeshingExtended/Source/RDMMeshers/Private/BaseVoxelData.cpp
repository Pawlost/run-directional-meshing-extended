#include "BaseVoxelData.h"

void UBaseVoxelData::BeginPlay()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	ChunkSpacing = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;

	Super::BeginPlay();
}

uint32 UBaseVoxelData::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

uint32 UBaseVoxelData::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

double UBaseVoxelData::GetChunkSpacing() const
{
	return ChunkSpacing;
}

double UBaseVoxelData::GetVoxelSize() const
{
	return VoxelSize;
}

uint32 UBaseVoxelData::GetVoxelCountPerVoxelLine() const
{
	return VoxelCountPerChunkDimension;
}

uint32 UBaseVoxelData::GetVoxelCountPerVoxelPlane() const
{
	return VoxelCountYZ;
}

uint32 UBaseVoxelData::GetVoxelCountPerChunk() const
{
	return VoxelCountXYZ;
}

double UBaseVoxelData::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkSpacing();
}