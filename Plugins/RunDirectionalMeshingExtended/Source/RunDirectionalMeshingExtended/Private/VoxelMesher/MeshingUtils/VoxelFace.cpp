#include "VoxelMesher/MeshingUtils/VoxelFace.h"

FVoxelFace FVoxelFace::CreateFrontFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght){
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition + FIntVector(0, 0, 0),
		InitialPosition + FIntVector(0, 0, 1),
		InitialPosition + FIntVector(0, RunLenght, 1));
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
		InitialPosition + FIntVector(0, RunLenght - 1, 0),
		 InitialPosition + FIntVector(1, RunLenght - 1, 0),
		InitialPosition + FIntVector(1, RunLenght - 1, 1),
		InitialPosition + FIntVector(0, RunLenght - 1, 1));
}

FVoxelFace FVoxelFace::CreateTopFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, 0, 0),
		 InitialPosition + FIntVector(0, RunLenght, 0),
		InitialPosition + FIntVector(1, RunLenght, 0),
		InitialPosition + FIntVector(1, 0, 0));
}

FVoxelFace FVoxelFace::CreateBottomFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght)
{
	return FVoxelFace(Voxel,
		InitialPosition + FIntVector(0, RunLenght, 0),
		 InitialPosition,
		InitialPosition + FIntVector(1, 0, 0),
		InitialPosition +FIntVector(1, RunLenght, 0));
}

void FVoxelFace::FrontBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, Y, X);
	OutQuadPosition = FIntVector(ChunkDimension, Y, X);
}

void FVoxelFace::BackBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, Y, X);
	OutQuadPosition = FIntVector(0, Y, X);
}

void FVoxelFace::LeftBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, X, Y);
	OutQuadPosition = FIntVector(X, 0, Y);
}

void FVoxelFace::RightBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, X, Y);
	OutQuadPosition = FIntVector(X, ChunkDimension, Y);
}

void FVoxelFace::TopBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, Y, X);
	OutQuadPosition =FIntVector(X, Y, ChunkDimension);
}

void FVoxelFace::BottomBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension)
{
	OutBorderLocation = FIntVector(0, Y, X);
	OutQuadPosition = FIntVector(X, Y, 0);
}

bool FVoxelFace::HeightConditionX(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return Face.StartVertexUp.X == NewFace.StartVertexUp.X;
}

bool FVoxelFace::HeightConditionY(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return Face.StartVertexUp.Y == NewFace.StartVertexUp.Y;
}

bool FVoxelFace::HeightConditionZ(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return Face.StartVertexUp.Z == NewFace.StartVertexUp.Z;
}

bool FVoxelFace::MergeFailConditionX(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return NewFace.StartVertexUp.Y < Face.StartVertexUp.Y &&
			   Face.StartVertexUp.Z > NewFace.StartVertexDown.Z;
}

bool FVoxelFace::MergeFailConditionY(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return NewFace.StartVertexUp.Z < Face.StartVertexUp.Z &&
			   Face.StartVertexUp.X > NewFace.StartVertexDown.X;
}

bool FVoxelFace::MergeFailConditionZ(const FVoxelFace& Face, const FVoxelFace& NewFace)
{
	return NewFace.StartVertexUp.Y < Face.StartVertexUp.Y &&
			   Face.StartVertexUp.X > NewFace.StartVertexDown.X;
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
 * @return true if previous face was merged into new
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

int FVoxelFace::GetXLocation(const FIntVector& Location)
{
	return Location.X;
}

int FVoxelFace::GetYLocation(const FIntVector& Location)
{
	return Location.Y;
}

int FVoxelFace::GetZLocation(const FIntVector& Location)
{
	return Location.Z;
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
