#pragma once
#include "MeshingDirections.h"

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
	FMeshingDirections FaceTemplates[CHUNK_FACE_COUNT] = {
		FMeshingDirections(FStaticMergeData::FrontFaceData), FMeshingDirections(FStaticMergeData::BackFaceData),
		FMeshingDirections(FStaticMergeData::RightFaceData), FMeshingDirections(FStaticMergeData::LeftFaceData),
		FMeshingDirections(FStaticMergeData::BottomFaceData), FMeshingDirections(FStaticMergeData::TopFaceData)
	};
	
	void AddFace(const int FaceIndex, const FVoxel Voxel, const FIntVector& Position,
		const int Lenght) const;
	
	FVirtualVoxelFaceContainer();
	explicit FVirtualVoxelFaceContainer(uint32 VoxelPlane);
	
	void CreateSideFace(const EFaceDirection FaceDirection,
		const FIntVector& VoxelPosition, const FVoxel& Voxel, const int YPosition);
	
private:
	TSharedPtr<TArray<FVirtualVoxelFace>> VirtualVoxelFaces;
};
