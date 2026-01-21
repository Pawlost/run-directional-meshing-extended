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
		for (uint32 y = 0; y < VoxelLine; y++)
		{
			VirtualFaces.Emplace(VoxelPlane);
		}
	}

	void GenerateVirtualFaces(FBorderParams& BorderParameters);

	void ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize);
	
	//void IncrementBorderRun(const EFaceDirection& FaceTemplate, const FIntVector& Position, int BorderIndex) const;
	//static bool IsBorderVoxelVisible(const FBorderVoxelIndexParams& FaceData);

	TSharedPtr<TArray<FVoxel>> VoxelGrid;
	
private:
	struct FVoxelParams
	{
		FIntVector Position = FIntVector(0);
		FVoxel* NextVoxel = nullptr;
	};
	
	bool InsertNextVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels, const EFaceDirection FaceIndex,
		bool CanGenerate, FIntVector PositionOffset) const;
	
	TArray<FVirtualVoxelFaceContainer> VirtualFaces;
	
	bool IsVoxelVisible(const int NextVoxelOffset);

	FORCEINLINE FVoxel& GetCurrentVoxel() const
	{
		return (*VoxelGrid)[VoxelIndex];
	}
	
	void IncrementRun(const EFaceDirection& FaceTemplate, int VoxelPlaneIndex, int NextVoxelOffset);
	
	static const TStaticArray<FIntVector, VOXEL_FACE_COUNT> IndexOffset;
	static const TStaticArray<EFaceDirection, VOXEL_FACE_COUNT> InverseDirections;
};
