#pragma once
#include "CoreMinimal.h"
#include "FaceMergeData.h"

struct FVoxelMesh;

struct FVirtualVoxelFaceContext
{
	void AddNewVirtualFace(const EFaceDirection FaceIndex, const FVoxel Voxel, const FIntVector& Position,
	                       const int Lenght);

	FVirtualVoxelFaceContext()
	{
	};
	explicit FVirtualVoxelFaceContext(uint32 VoxelPlane);

	void DirectionalGreedyMergeForVoxelPlane(
		TArray<FVirtualVoxelFace>* ActiveArray, TArray<FVirtualVoxelFace>* PassiveArray,
		FVoxelMesh& VoxelMeshContainer,const double VoxelSize, const int MaxVoxelsInChunk);

private:
	
	static const FFaceMergeData FrontFaceData;
	static const FFaceMergeData BackFaceData;
	static const FFaceMergeData LeftFaceData;
	static const FFaceMergeData RightFaceData;
	static const FFaceMergeData TopFaceData;
	static const FFaceMergeData BottomFaceData;
	
	/*
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Top = 4,
	Bottom = 5,
	*/
	static const FFaceMergeData MeshingDataArray[VOXEL_FACE_COUNT];
	
	TStaticArray<TArray<FVirtualVoxelFace>, VOXEL_FACE_COUNT> FacesByDirection;
};
