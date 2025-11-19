#pragma once
#include "CoreMinimal.h"
#include "Voxel/Voxel.h"

#define CHUNK_FACE_COUNT 6

/**
 * Struct representing single voxel face. Inside helper struct not intended to be used with Unreal Engine.
 */
struct RDMMESHERS_API FVirtualVoxelFace
{
	FVoxel Voxel;
	FIntVector StartVertexDown;
	FIntVector EndVertexDown;
	FIntVector EndVertexUp;
	FIntVector StartVertexUp;

	FVirtualVoxelFace() : Voxel(FVoxel()), StartVertexDown(), EndVertexDown(), EndVertexUp(), StartVertexUp()
	{
	}

	FVirtualVoxelFace(const FVoxel& Voxel, const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : Voxel(Voxel),
		StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	FVirtualVoxelFace(const FIntVector& StartVertexDown, const FIntVector& EndVertexDown,
	           const FIntVector& EndVertexUp, const FIntVector& StartVertexUp) : StartVertexDown(StartVertexDown),
		EndVertexDown(EndVertexDown), EndVertexUp(EndVertexUp), StartVertexUp(StartVertexUp)
	{
	}

	static FVirtualVoxelFace CreateFrontFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVirtualVoxelFace CreateBackFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVirtualVoxelFace CreateLeftFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVirtualVoxelFace CreateRightFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVirtualVoxelFace CreateTopFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);
	static FVirtualVoxelFace CreateBottomFace(const FVoxel& Voxel, const FIntVector& InitialPosition, const int RunLenght = 1);

	
	static void FrontBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);
	static void BackBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);
	static void LeftBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);
	static void RightBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);
	static void TopBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);
	static void BottomBorderLocation(FIntVector& OutBorderLocation, FIntVector& OutQuadPosition, int X, int Y, int ChunkDimension);

	static bool HeightConditionX(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	static bool HeightConditionY(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	static bool HeightConditionZ(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	
	static bool MergeFailConditionX(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	static bool MergeFailConditionRightY(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	static bool MergeFailConditionLeftY(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	static bool MergeFailConditionZ(const FVirtualVoxelFace& Face, const FVirtualVoxelFace& NewFace);
	
	static bool MergeFaceEnd(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace);
	static bool MergeFaceStart(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace);
	static bool MergeFaceUp(FVirtualVoxelFace& PrevFace, const FVirtualVoxelFace& NewFace);
	
	static int GetXLocation(const FIntVector& Location);
	static int GetYLocation(const FIntVector& Location);
	static int GetZLocation(const FIntVector& Location);

	FVector GetFinalStartVertexDown(const double& VoxelSize) const;
	FVector GetFinalStartVertexUp(const double& VoxelSize) const;
	FVector GetFinalEndVertexDown(const double& VoxelSize) const;
	FVector GetFinalEndVertexUp(const double& VoxelSize) const;
};
