#pragma once
#include "VirtualMeshEventPlanner.h"
#include "VoxelIndexCounter.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFaceContainer.h"

struct FBasicRDMVirtualMesher : FVoxelIndexCounter
{
	struct FBorderVoxelIndexParams
	{
		int32 SideChunkVoxelIndex;
		FStaticMergeData StaticData;
	};

	FBasicRDMVirtualMesher(uint32 VoxelLine, uint32 VoxelPlane, uint32 MaxVoxelsInChunk) : 
		FVoxelIndexCounter(VoxelLine, VoxelPlane, MaxVoxelsInChunk)
	{
	}

	void GenerateVirtualFaces(FBorderParams& BorderParameters);

	bool IsVoxelVisible(const int NextVoxelOffset);

	FORCEINLINE FVoxel& GetCurrentVoxel() const
	{
		return (*VoxelGrid)[CurrentIndex];
	}

	void ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize);

	void IncrementRun(const EFaceDirection& FaceTemplate, int VoxelPlaneIndex, int NextVoxelOffset);

	//void IncrementBorderRun(const EFaceDirection& FaceTemplate, const FIntVector& Position, int BorderIndex) const;
	//static bool IsBorderVoxelVisible(const FBorderVoxelIndexParams& FaceData);

private:
	uint32 CurrentIndex = 0;
	FIntVector VoxelPosition;

	TArray<FVirtualVoxelFaceContainer> VirtualFaces;
	TSharedPtr<TArray<FVoxel>> VoxelGrid;
};
