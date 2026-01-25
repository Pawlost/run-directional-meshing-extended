#pragma once
#include "CoreMinimal.h"
#include "StaticMergeData.h"

struct FVoxelMeshContainer;

struct FVirtualVoxelFaceContainer
{
	void AddNewVirtualFace(const EFaceDirection FaceIndex, const FVoxel Voxel, const FIntVector& Position,
	                       const int Lenght);

	FVirtualVoxelFaceContainer()
	{
	};
	explicit FVirtualVoxelFaceContainer(uint32 VoxelPlane);

	void DirectionalGreedyMergeForVoxelPlane(
		TArray<FVirtualVoxelFace>* ActiveArray, TArray<FVirtualVoxelFace>* PassiveArray,
		FVoxelMeshContainer& VoxelMeshContainer,const double VoxelSize, const int MaxVoxelsInChunk);

private:
	
	/*
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Top = 4,
	Bottom = 5,
	*/
	static const FStaticMergeData MeshingDataArray[VOXEL_FACE_COUNT];
	
	TStaticArray<TArray<FVirtualVoxelFace>, VOXEL_FACE_COUNT> VirtualVoxelFaceContainer;
};
