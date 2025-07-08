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
		Vertices.Reserve(VoxelCountPerChunk);
		Triangles.Reserve(VoxelCountPerChunk);
		UV0.Reserve(VoxelCountPerChunk);
		Normals.Reserve(VoxelCountPerChunk);
		Tangents.Reserve(VoxelCountPerChunk);
	}
};
