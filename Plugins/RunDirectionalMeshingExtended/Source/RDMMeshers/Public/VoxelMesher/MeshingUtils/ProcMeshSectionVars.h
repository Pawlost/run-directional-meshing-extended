#pragma once
#include "ProceduralMeshComponent.h"

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
		Vertices.Reserve(VoxelCountPerChunk * 12 + 1);
		Triangles.Reserve(VoxelCountPerChunk * 24 + 1);
		UV0.Reserve(VoxelCountPerChunk * 24 + 1);
		Normals.Reserve(VoxelCountPerChunk * 12 + 1);
		Tangents.Reserve(VoxelCountPerChunk * 12 + 1);
	}

	void EmptyValues()
	{
		Vertices.Empty();
		Triangles.Empty();
		UV0.Empty();
		Normals.Empty();
		Tangents.Empty();
	}
};
