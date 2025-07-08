#pragma once

#include "ProcMeshSectionVars.generated.h"

USTRUCT()
struct FProcMeshSectionVars
{
	GENERATED_BODY()
	
	TArray<FVector> Vertices; 
	TArray<int32> Triangles;
	TArray<FVector2D> UV0;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	int32 GlobalTriangleIndex;
	
	FProcMeshSectionVars() : GlobalTriangleIndex(0) {}
	
	explicit FProcMeshSectionVars(const int VoxelCountPerChunk): GlobalTriangleIndex(0)
	{
		constexpr int VERTICES_PER_VOXEL = 24;
		const int VERTICES_PER_CHUNK = VoxelCountPerChunk * VERTICES_PER_VOXEL;

		Vertices.Reserve(VERTICES_PER_CHUNK);
		Triangles.Reserve(VERTICES_PER_CHUNK);
		UV0.Reserve(VERTICES_PER_CHUNK);
		Normals.Reserve(VERTICES_PER_CHUNK);
		Tangents.Reserve(VERTICES_PER_CHUNK);
	}
};
