#pragma once
#include "CoreMinimal.h"
#include "StaticMergeData.h"

struct FVoxelMeshContainer;

struct FVirtualVoxelFaceContainer
{	
	/*
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Bottom = 4,
	Top = 5
	*/
	static FStaticMergeData MeshingDataArray[CHUNK_FACE_COUNT];
	
	void AddNewVirtualFace(const EFaceDirection FaceIndex, const FVoxel Voxel, const FIntVector& Position,
		const int Lenght);
	
	FVirtualVoxelFaceContainer(){};
	explicit FVirtualVoxelFaceContainer(uint32 VoxelPlane);
	
	void DirectionalGreedyMergeForVoxelPlane(
		TArray<FVirtualVoxelFace>& FirstArray, TArray<FVirtualVoxelFace>& SecondArray, FVoxelMeshContainer& VoxelMeshContainer,
		 const EFaceDirection FaceDirection, const double VoxelSize, const int MaxVoxelsInChunk);
	
	TStaticArray<TArray<FVirtualVoxelFace>, CHUNK_FACE_COUNT>  VirtualVoxelFaces;
};
