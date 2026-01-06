#pragma once
#include "CoreMinimal.h"
#include "MeshingDirections.h"

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
	FStaticMergeData MeshingDataArray[CHUNK_FACE_COUNT] = {
		FStaticMergeData::FrontFaceData, FStaticMergeData::BackFaceData,
		FStaticMergeData::RightFaceData, FStaticMergeData::LeftFaceData,
		FStaticMergeData::BottomFaceData, FStaticMergeData::TopFaceData
	};
	
	void AddFace(const int FaceIndex, const FVoxel Voxel, const FIntVector& Position,
		const int Lenght);
	
	FVirtualVoxelFaceContainer(){};
	explicit FVirtualVoxelFaceContainer(uint32 VoxelPlane);
	
	void CreateSideFace(const EFaceDirection FaceDirection,
		const FIntVector& VoxelPosition, const FVoxel& Voxel, const int YPosition);
	
	void DirectionalGreedyMergeForVoxelPlane(
		TArray<FVirtualVoxelFace>& FirstArray, TArray<FVirtualVoxelFace>& SecondArray, FVoxelMeshContainer& VoxelMeshContainer,
		 const EFaceDirection FaceDirection, const double VoxelSize, const int MaxVoxelsInChunk);
	
private:
	TArray<FVirtualVoxelFace> VirtualVoxelFaces;
};
