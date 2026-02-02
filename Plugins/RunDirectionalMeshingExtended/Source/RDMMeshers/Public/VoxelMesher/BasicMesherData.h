#pragma once
#include "BaseVoxelDataDummy.h"
#include "MeshingUtil/VirtualVoxelFace.h"
#include "MeshingUtil/Enum/FaceDirection.h"

struct FBasicMesherData
{

public:
	FBasicMesherData(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
		: VoxelLine(BaseVoxelData == nullptr ? 0 : BaseVoxelData->GetVoxelLine()), 
		VoxelPlane(BaseVoxelData == nullptr ? 0 : BaseVoxelData->GetVoxelPlane()),
		MaxVoxelsInChunk(BaseVoxelData == nullptr ? 0 : BaseVoxelData->GetMaxVoxelsInChunk())
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
