#include "VoxelMesher/MeshingUtils/VoxelFace.h"

FVoxelFace FVoxelFace::CreateFrontFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght){
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(1, RunLenght, 0),
		 InitialPosition + FIntVector(1, 0, 0),
		InitialPosition + FIntVector(1, 0, 1),
		InitialPosition + FIntVector(1, RunLenght, 1));
}


FVoxelFace FVoxelFace::CreateBackFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition,
		 InitialPosition + FIntVector(0, RunLenght, 0),
		InitialPosition + FIntVector(0, RunLenght, 1),
		InitialPosition + FIntVector(0, 0, 1));
}


FVoxelFace FVoxelFace::CreateLeftFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(1, 0, 0),
		 InitialPosition,
		InitialPosition + FIntVector(0, 0, 1),
		InitialPosition + FIntVector(1, 0, 1));
}

FVoxelFace FVoxelFace::CreateRightFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition + FIntVector(1, RunLenght, 0),
		InitialPosition + FIntVector(1, RunLenght, 1),
		InitialPosition + FIntVector(0, RunLenght, 1));
}

FVoxelFace FVoxelFace::CreateTopFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, 0, 1),
		 InitialPosition + FIntVector(0, RunLenght, 1),
		InitialPosition + FIntVector(1, RunLenght, 1),
		InitialPosition + FIntVector(1, 0, 1));
}

FVoxelFace FVoxelFace::CreateBottomFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition,
		InitialPosition + FIntVector(1, 0, 0),
		InitialPosition +FIntVector(1, RunLenght, 0));
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FVoxelFace::MergeFaceEnd(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
{
	if (PrevFace.EndVertexDown == NewFace.StartVertexDown &&
		PrevFace.EndVertexUp == NewFace.StartVertexUp)
	{
		PrevFace.EndVertexDown = NewFace.EndVertexDown;
		PrevFace.EndVertexUp = NewFace.EndVertexUp;
		return true;
	}
	return false;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FVoxelFace::MergeFaceStart(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
{
	if (PrevFace.StartVertexUp == NewFace.EndVertexUp &&
		PrevFace.StartVertexDown == NewFace.EndVertexDown)
	{
		PrevFace.StartVertexDown = NewFace.StartVertexDown;
		PrevFace.StartVertexUp = NewFace.StartVertexUp;
		return true;
	}
	return false;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FVoxelFace::MergeFaceUp(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
{
	if (PrevFace.StartVertexUp == NewFace.StartVertexDown &&
		PrevFace.EndVertexUp == NewFace.EndVertexDown)
	{
		PrevFace.StartVertexUp = NewFace.StartVertexUp;
		PrevFace.EndVertexUp = NewFace.EndVertexUp;
		return true;
	}
	return false;
}

FVector3f FVoxelFace::GetFinalStartVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector3f>(StartVertexDown) * VoxelSize;
}

FVector3f FVoxelFace::GetFinalStartVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector3f>(StartVertexUp) * VoxelSize;
}

FVector3f FVoxelFace::GetFinalEndVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector3f>(EndVertexDown) * VoxelSize;
}

FVector3f FVoxelFace::GetFinalEndVertexUp(const double& voxelSize) const
{
	return static_cast<FVector3f>(EndVertexUp) * voxelSize;
}
