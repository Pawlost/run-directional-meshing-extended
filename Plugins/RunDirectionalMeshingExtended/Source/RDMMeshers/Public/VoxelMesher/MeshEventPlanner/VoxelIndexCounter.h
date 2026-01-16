#pragma once
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h"

struct FVoxelIndexCounter
{
	public:
	
	FVoxelIndexCounter(uint32 VoxelLine, uint32 VoxelPlane, uint32 MaxVoxelsInChunk)
		: VoxelLine(VoxelLine), VoxelPlane(VoxelPlane), MaxVoxelsInChunk(MaxVoxelsInChunk)
	{
		constexpr int EstimatedRows = 3;

		FirstDirectionalMeshingHelperArray.Reserve(VoxelLine * EstimatedRows);
		SecondDirectionalMeshingHelperArray.Reserve(VoxelLine * EstimatedRows);
	}

	// Current event index made of all meshing events that were already processed/traversed.
	uint32 CurrentVoxelIndex = 0;
	
protected:
	uint32 VoxelLine = 0;
	uint32 VoxelPlane = 0;
	uint32 MaxVoxelsInChunk = 0;
	
	FIntVector CurrentVoxelPosition = FIntVector(0, 0, 0);

	void UpdatePositionFromIndex();
	void UpdateIndexFromPosition();
	
	TArray<FVirtualVoxelFace> FirstDirectionalMeshingHelperArray;
	TArray<FVirtualVoxelFace> SecondDirectionalMeshingHelperArray;
};
