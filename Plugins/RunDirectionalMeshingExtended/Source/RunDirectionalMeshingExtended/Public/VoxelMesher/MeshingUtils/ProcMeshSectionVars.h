#pragma once

#include "ProcMeshSectionVars.generated.h"

USTRUCT()
struct FProcMeshSectionVars
{
	GENERATED_BODY()
	
	TSharedPtr<TArray<FVector>> Vertices; 
	TSharedPtr<TArray<int32>> Triangles;
	TSharedPtr<TArray<FVector2D>> UV0;
	TSharedPtr<TArray<FVector>> Normals ;
	TSharedPtr<TArray<FProcMeshTangent>> Tangents;
	int32 GlobalTriangleIndex;
	
	FProcMeshSectionVars() : GlobalTriangleIndex(0) {}
	
	explicit FProcMeshSectionVars(const int VoxelCountPerChunk): GlobalTriangleIndex(0)
	{
		constexpr int VERTICES_PER_VOXEL = 24;
		const int VERTICES_PER_CHUNK = VoxelCountPerChunk * VERTICES_PER_VOXEL;

		Vertices = MakeShared<TArray<FVector>>();
		Vertices->Reserve(VERTICES_PER_CHUNK);

		Triangles = MakeShared<TArray<int32>>();
		Triangles->Reserve(VERTICES_PER_CHUNK);

		UV0 = MakeShared<TArray<FVector2D>>();
		UV0->Reserve(VERTICES_PER_CHUNK);

		Normals = MakeShared<TArray<FVector>>();
		Normals->Reserve(VERTICES_PER_CHUNK);

		Tangents = MakeShared<TArray<FProcMeshTangent>>();
		Tangents->Reserve(VERTICES_PER_CHUNK);
	}

	bool IsValid() const
	{
		return Vertices.IsValid() && Triangles.IsValid() && UV0.IsValid() && Normals.IsValid() && Tangents.IsValid();
	}
};
