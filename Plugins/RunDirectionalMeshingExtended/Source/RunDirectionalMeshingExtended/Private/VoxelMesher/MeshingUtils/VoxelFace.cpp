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

bool FVoxelFace::FaceRowConditionZ(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return Face.StartVertexUp.Z < NewFace.StartVertexDown.Z;
}


bool FVoxelFace::FaceRowConditionX(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return Face.StartVertexUp.X < NewFace.StartVertexDown.X;
}

/**
 * Compare vertices and merge quads
 * @return true if previous face was merged
 */
bool FVoxelFace::MergeFaceEnd(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
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
bool FVoxelFace::MergeFaceStart(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
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
 * @return true if previous face was merged
 */
bool FVoxelFace::MergeFaceUp(FVoxelFace& PrevFace, const FVoxelFace& NewFace)
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

FVector FVoxelFace::GetFinalStartVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector>(StartVertexDown) * VoxelSize;
}

FVector FVoxelFace::GetFinalStartVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector>(StartVertexUp) * VoxelSize;
}

FVector FVoxelFace::GetFinalEndVertexDown(const double& VoxelSize) const
{
	return static_cast<FVector>(EndVertexDown) * VoxelSize;
}

FVector FVoxelFace::GetFinalEndVertexUp(const double& VoxelSize) const
{
	return static_cast<FVector>(EndVertexUp) * VoxelSize;
}
