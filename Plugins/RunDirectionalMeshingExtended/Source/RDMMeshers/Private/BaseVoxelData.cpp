#include "BaseVoxelData.h"

/// Called when the actor begins play. Initializes voxel data calculations.
void UBaseVoxelData::BeginPlay()
{
	CalculateVoxelData();
	Super::BeginPlay();
}

/// Calculates and caches voxel grid dimensions and spacing values.
/// This function computes:
/// - VoxelCountY: Number of voxels along one dimension
/// - ChunkSpacing: Total spatial size of the chunk in world units
/// - VoxelCountYZ: Total voxels in a Y-Z plane (for efficient indexing)
/// - VoxelCountXYZ: Total voxels in the entire chunk
void UBaseVoxelData::CalculateVoxelData()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	
	// Calculate the total spatial size of the chunk (in world units)
	ChunkSpacing = VoxelCountY * VoxelSize;
	
	// Pre-calculate Y-Z plane size for faster 3D-to-1D index conversion
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	
	// Pre-calculate total voxel count for the entire chunk
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;
}

/// Converts a 3D voxel position to a linear 1D array index.
/// Uses row-major order with Y as the innermost dimension.
/// 
/// @param X The X coordinate of the voxel position
/// @param Y The Y coordinate of the voxel position
/// @param Z The Z coordinate of the voxel position
/// @return The linear index into a 1D voxel array
uint32 UBaseVoxelData::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

/// Converts a 3D voxel position (as FIntVector) to a linear 1D array index.
/// This is an overload that delegates to the direct parameter version.
/// 
/// @param VoxelPosition The 3D coordinates of the voxel position
/// @return The linear index into a 1D voxel array
uint32 UBaseVoxelData::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

/// Gets the total spatial size of the chunk in world units.
/// 
/// @return The chunk spacing (ChunkSpacing) in world units
double UBaseVoxelData::GetChunkSpacing() const
{
	return ChunkSpacing;
}

/// Gets the size of individual voxels in world units.
/// 
/// @return The voxel size in world units
double UBaseVoxelData::GetVoxelSize() const
{
	return VoxelSize;
}

/// Gets the number of voxels along a single dimension of the chunk.
/// 
/// @return The voxel count per chunk dimension (e.g., 32 for a 32x32x32 chunk)
uint32 UBaseVoxelData::GetVoxelLine() const
{
	return VoxelCountPerChunkDimension;
}

/// Gets the total number of voxels in a 2D plane (Y-Z plane).
/// Useful for 3D-to-1D index calculations.
/// 
/// @return The number of voxels in a Y-Z plane (VoxelCountY²)
uint32 UBaseVoxelData::GetVoxelPlane() const
{
	return VoxelCountYZ;
}

/// Gets the maximum total number of voxels that can fit in a chunk.
/// 
/// @return The total number of voxels in the chunk (VoxelCountY³)
uint32 UBaseVoxelData::GetMaxVoxelsInChunk() const
{
	return VoxelCountXYZ;
}

/// Gets the highest elevation at a given world location.
/// Currently returns the chunk spacing as a placeholder implementation.
/// 
/// @param Location The world location to query
/// @return The highest elevation (currently returns chunk spacing)
double UBaseVoxelData::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkSpacing();
}
