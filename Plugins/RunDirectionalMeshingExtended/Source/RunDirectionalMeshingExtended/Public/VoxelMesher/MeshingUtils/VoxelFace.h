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

	
	static FIntVector FrontBorderLocation(const int X, const int Y, const int ChunkDimension);
	static FIntVector BackBorderLocation(const int X, const int Y, const int ChunkDimension);
	static FIntVector LeftBorderLocation(const int X, const int Y, const int ChunkDimension);
	static FIntVector RightBorderLocation(const int X, const int Y, const int ChunkDimension);
	static FIntVector TopBorderLocation(const int X, const int Y, const int ChunkDimension);
	static FIntVector BottomBorderLocation(const int X, const int Y, const int ChunkDimension);

	static bool FaceRowConditionZ(const FVoxelFace& Face, const FVoxelFace& NewFace);
	static bool FaceRowConditionX(const FVoxelFace& Face, const FVoxelFace& NewFace);
	
	static bool MergeFaceEnd(FVoxelFace& PrevFace, const FVoxelFace& NewFace);
	static bool MergeFaceStart(FVoxelFace& PrevFace, const FVoxelFace& NewFace);
	static bool MergeFaceUp(FVoxelFace& PrevFace, const FVoxelFace& NewFace);
	
	static int GetXLocation(const FIntVector& Location);
	static int GetYLocation(const FIntVector& Location);
	static int GetZLocation(const FIntVector& Location);

	FVector GetFinalStartVertexDown(const double& VoxelSize) const;
	FVector GetFinalStartVertexUp(const double& VoxelSize) const;
	FVector GetFinalEndVertexDown(const double& VoxelSize) const;
	FVector GetFinalEndVertexUp(const double& VoxelSize) const;
};
