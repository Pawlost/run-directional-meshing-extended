#pragma once
#include "VoxelEventMesher.h"
#include "BasicMesherData.h"

struct FBasicVoxelMesher : FBasicMesherData
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
		FFaceMergeData StaticData;
	};
	
	const static TStaticArray<FVoxelSideParams, VOXEL_FACE_COUNT> VoxelSideParams;

	FBasicVoxelMesher(const TObjectPtr<UBaseVoxelData> BaseVoxelData);

	void GenerateVirtualFaces(FChunkBorderContext& BorderParameters);

	void ConvertVirtualFacesToMesh(FVoxelMesh& VoxelMeshContainer, const double VoxelSize);
	
	void CheckBorder(FChunkBorderContext& BorderContext, EFaceDirection Direction, const FIntVector& CurrentPosition, const FIntVector& BorderVoxelPosition);
	
	TSharedPtr<TArray<FVoxel>> VoxelGrid;
	
private:
	
	TStaticArray<FVoxel*, VOXEL_FACE_COUNT> TempVoxels;
	
	bool CheckInnerVoxel(const EFaceDirection FaceIndex, bool CanGenerate);;
	
	TArray<FVirtualVoxelFaceContext> VirtualFaces;

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
