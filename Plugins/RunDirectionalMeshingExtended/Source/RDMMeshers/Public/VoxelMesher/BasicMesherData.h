#pragma once
#include "BaseVoxelData.h"
#include "MeshingUtil/VirtualVoxelFace.h"
#include "MeshingUtil/Enum/FaceDirection.h"

struct RDMMESHERS_API FBasicMesherData
{

public:
	FBasicMesherData(const TObjectPtr<UBaseVoxelData> BaseVoxelData);

	uint32 CalculateIndexFromPosition(const FIntVector& Position) const;
	
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
	
	TArray<FVirtualVoxelFace> FirstDirectionalMeshingHelperArray;
	TArray<FVirtualVoxelFace> SecondDirectionalMeshingHelperArray;
};
