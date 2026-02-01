#pragma once
#include "MeshingUtil/VirtualVoxelFace.h"
#include "MeshingUtil/Enum/FaceDirection.h"

struct FBasicMesherData
{

public:
	FBasicMesherData(const uint32 VoxelLine, const uint32 VoxelPlane, const uint32 MaxVoxelsInChunk)
		: VoxelLine(VoxelLine), VoxelPlane(VoxelPlane), MaxVoxelsInChunk(MaxVoxelsInChunk)
	{
		constexpr int EstimatedRows = 3;

		FirstDirectionalMeshingHelperArray.Reserve(VoxelLine * EstimatedRows);
		SecondDirectionalMeshingHelperArray.Reserve(VoxelLine * EstimatedRows);
	}

	
protected:
	const static TStaticArray<FIntVector, VOXEL_FACE_COUNT> VoxelPositionOffsets;
	
	uint32 VoxelLine = 0;
	uint32 VoxelPlane = 0;
	uint32 MaxVoxelsInChunk = 0;
	
	// Current event index made of all meshing events that were already processed/traversed.
	uint32 VoxelIndex = 0;
	
	FIntVector VoxelPosition = FIntVector(0, 0, 0);

	void UpdatePositionFromIndex();
	void UpdateIndexFromPosition();
	
	uint32 CalculateIndexFromPosition(const FIntVector& Position) const;
	
	TArray<FVirtualVoxelFace> FirstDirectionalMeshingHelperArray;
	TArray<FVirtualVoxelFace> SecondDirectionalMeshingHelperArray;
};
