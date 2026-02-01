#pragma once
#include "ProcMeshSectionVars.h"
#include "VirtualVoxelFace.h"
#include "Enum/FaceDirection.h"
#include "Voxel/Voxel.h"

struct FVoxelMesh
{
	FVoxelMesh()
	{
		constexpr int EstimatedVoxelRows = 100;
		VoxelTable.Reserve(EstimatedVoxelRows);
	}
	
	struct FNormalsAndTangents
	{
		FVector Normal;
		FProcMeshTangent Tangent;
	};
	
	static const FNormalsAndTangents FaceNormalsAndTangents[VOXEL_FACE_COUNT];
	
	TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>> VoxelTable;
	
	void AddVirtualFaceToMesh(const FVirtualVoxelFace& Face, EFaceDirection FaceIndex, double VoxelSize,
		int MaxNumberVoxels);
	
	FORCEINLINE void Empty()
	{
		VoxelTable.Empty();
	}
};
