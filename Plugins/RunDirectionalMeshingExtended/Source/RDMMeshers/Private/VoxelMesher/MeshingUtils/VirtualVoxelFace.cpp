#include "VoxelMesher/MeshingUtil/VirtualVoxelFace.h"

FVirtualVoxelFace FVirtualVoxelFace::CreateFrontFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght){
	return FVirtualVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition + FIntVector(0, 0, 0),
		InitialPosition + FIntVector(0, 0, 1),
		InitialPosition + FIntVector(0, RunLenght, 1));
}


FVirtualVoxelFace FVirtualVoxelFace::CreateBackFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVirtualVoxelFace(Voxel,
		InitialPosition,
		 InitialPosition + FIntVector(0, RunLenght, 0),
		InitialPosition + FIntVector(0, RunLenght, 1),
		InitialPosition + FIntVector(0, 0, 1));
}

FVirtualVoxelFace FVirtualVoxelFace::CreateLeftFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVirtualVoxelFace(Voxel,
		InitialPosition + FIntVector(1, 0, 0),
		 InitialPosition,
		InitialPosition + FIntVector(0, 0, 1),
		InitialPosition + FIntVector(1, 0, 1));
}

FVirtualVoxelFace FVirtualVoxelFace::CreateRightFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVirtualVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght - 1, 0),
		 InitialPosition + FIntVector(1, RunLenght - 1, 0),
		InitialPosition + FIntVector(1, RunLenght - 1, 1),
		InitialPosition + FIntVector(0, RunLenght - 1, 1));
}

FVirtualVoxelFace FVirtualVoxelFace::CreateTopFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVirtualVoxelFace(Voxel,
		InitialPosition + FIntVector(0, 0, 0),
		 InitialPosition + FIntVector(0, RunLenght, 0),
		InitialPosition + FIntVector(1, RunLenght, 0),
		InitialPosition + FIntVector(1, 0, 0));
}

FVirtualVoxelFace FVirtualVoxelFace::CreateBottomFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVirtualVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition,
		InitialPosition + FIntVector(1, 0, 0),
		InitialPosition +FIntVector(1, RunLenght, 0));
}

bool FVirtualVoxelFace::MergeFailConditionX(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)
{
	return  Face.StartVertexDown.Z > NewFace.StartVertexUp.Z;
}

bool FVirtualVoxelFace::MergeFailConditionRightY(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)
{
	return Face.StartVertexUp.X > NewFace.EndVertexDown.X;
}

bool FVirtualVoxelFace::MergeFailConditionLeftY(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)
{
	return Face.EndVertexUp.X > NewFace.StartVertexUp.X;
}

bool FVirtualVoxelFace::MergeFailConditionZ(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace)
{
	return Face.StartVertexDown.X > NewFace.StartVertexUp.X;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FVirtualVoxelFace::MergeFaceEnd(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)
{
	if (PrevFace.Voxel == NewFace.Voxel &&
		PrevFace.EndVertexDown == NewFace.StartVertexDown &&
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
bool FVirtualVoxelFace::MergeFaceStart(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)
{
	if (PrevFace.Voxel == NewFace.Voxel &&
		PrevFace.StartVertexUp == NewFace.EndVertexUp &&
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
 * @return true if previous face was merged into new
 */
bool FVirtualVoxelFace::MergeFaceUp(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace)
{
	if (PrevFace.Voxel == NewFace.Voxel &&
		PrevFace.StartVertexUp == NewFace.StartVertexDown &&
		PrevFace.EndVertexUp == NewFace.EndVertexDown)
	{
		PrevFace.StartVertexUp = NewFace.StartVertexUp;
		PrevFace.EndVertexUp = NewFace.EndVertexUp;
		return true;
	}
	return false;
}

int FVirtualVoxelFace::GetXFromVector(const FIntVector& Position)
{
	return Position.X;
}

int FVirtualVoxelFace::GetYFromVector(const FIntVector& Position)
{
	return Position.Y;
}

int FVirtualVoxelFace::GetZFromVector(const FIntVector& Position)
{
	return Position.Z;
}

FVector FVirtualVoxelFace::GetFinalStartVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector>(StartVertexDown) * VoxelSize;
}

FVector FVirtualVoxelFace::GetFinalStartVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector>(StartVertexUp) * VoxelSize;
}

FVector FVirtualVoxelFace::GetFinalEndVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector>(EndVertexDown) * VoxelSize;
}

FVector FVirtualVoxelFace::GetFinalEndVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector>(EndVertexUp) * VoxelSize;
}
