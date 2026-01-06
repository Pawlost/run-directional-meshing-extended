#pragma once
#include "FaceDirection.h"
#include "ProcMeshSectionVars.h"
#include "VirtualVoxelFace.h"
#include "Voxel/Voxel.h"

struct FVoxelMeshContainer
{
	FVoxelMeshContainer()
	{
		constexpr int EstimatedVoxelRows = 100;
		VoxelTable.Reserve(EstimatedVoxelRows);
	}
	
	struct FNormalsAndTangents
	{
		FVector Normal;
		FProcMeshTangent Tangent;
	};
	
	static const FNormalsAndTangents FaceNormalsAndTangents[CHUNK_FACE_COUNT];
	
	TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>> VoxelTable;
	
	void AddVirtualFaceToMesh(const FVirtualVoxelFace& Face, EFaceDirection FaceIndex, double VoxelSize,
		int MaxNumberVoxels);
	
	FORCEINLINE void Empty()
	{
		VoxelTable.Empty();
	}
};
