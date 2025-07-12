#pragma once
#include "CoreMinimal.h"
#include "Voxel/Voxel.h"

/**
 * Struct representing single voxel face. Inside helper struct not intended to be used with Unreal Engine.
 */
struct RUNDIRECTIONALMESHINGEXTENDED_API FVoxelFace
{
	FVoxel Voxel;
	FIntVector StartVertexDown;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;
	FIntVector StartVertexUp;

	FVoxelFace() : Voxel(FVoxel()), StartVertexDown(), EndVertexDown(), EndVertexUp(), StartVertexUp()
	{
	}

	FVoxelFace(const FVoxel& Voxel, const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : Voxel(Voxel),
		StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	FVoxelFace(const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	static FVoxelFace CreateFrontFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVoxelFace CreateBackFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVoxelFace CreateLeftFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVoxelFace CreateRightFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVoxelFace CreateTopFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVoxelFace CreateBottomFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);

	static bool FrontFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool BackFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool LeftFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool RightFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool TopFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool BottomFaceRowCondition(const FVoxelFace& Face, const int Coord1, const int Coord2);

	static bool FrontFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool BackFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool LeftFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool RightFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool TopFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	static bool BottomFaceNextRow(const FVoxelFace& Face, const int Coord1, const int Coord2);
	
	static bool MergeFaceEnd(FVoxelFace& PrevFace, const FVoxelFace& NewFace);
	static bool MergeFaceStart(FVoxelFace& PrevFace, const FVoxelFace& NewFace);
	static bool MergeFaceUp(FVoxelFace& PrevFace, const FVoxelFace& NewFace);

	FVector GetFinalStartVertexDown(const double& VoxelSize) const;
	FVector GetFinalStartVertexUp(const double& VoxelSize) const;
	FVector GetFinalEndVertexDown(const double& VoxelSize) const;
	FVector GetFinalEndVertexUp(const double& VoxelSize) const;
};
