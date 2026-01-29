#pragma once
#include "VirtualMeshEventPlanner.h"
#include "VoxelIndexCounter.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFaceContainer.h"

struct FBasicRDMVirtualMesher : FVoxelIndexCounter
{
	struct FVoxelSideParams
	{
		EFaceDirection FaceDirection;
		EFaceDirection InverseDirection;
		FIntVector IndexOffset;
		TFunctionRef<int(const FIntVector& Position)> GetVoxelPlaneFromPosition;
	};
	
	struct FBorderVoxelIndexParams
	{
		int32 SideChunkVoxelIndex;
		FStaticMergeData StaticData;
	};
	
	const static TStaticArray<FVoxelSideParams, VOXEL_FACE_COUNT> VoxelSideParams;

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
	
	void CheckBorder(FBorderParams& BorderParameters, EFaceDirection Direction, const FIntVector& CurrentPosition, const FIntVector& BorderVoxelPosition);
	
	TSharedPtr<TArray<FVoxel>> VoxelGrid;
	
private:
	struct FVoxelParams
	{
		FVoxel NextVoxel;
		bool IsBorderVoxel = false;
		bool IsValid = false;
	};
	
	bool CheckInnerVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels, const EFaceDirection FaceIndex,
		bool CanGenerate) const;
	
	TArray<FVirtualVoxelFaceContainer> VirtualFaces;

	FORCEINLINE FVoxel& GetCurrentVoxel() const
	{
		return (*VoxelGrid)[VoxelIndex];
	}
	
	FORCEINLINE FVoxel& GetVoxelFromPosition(const FIntVector& Position) const
	{
		const auto VoxelPositionIndex = CalculateIndexFromPosition(Position);
		return (*VoxelGrid)[VoxelPositionIndex];
	}
};
