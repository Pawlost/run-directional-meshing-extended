#include "VoxelMesher/MeshingUtils/VoxelMeshContainer.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFace.h"

const FVoxelMeshContainer::FNormalsAndTangents FVoxelMeshContainer::FaceNormalsAndTangents[] = {
	{FVector(1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 1.0, 0.0)}, //Front
	{FVector(-1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 1.0, 0.0)}, //Back
	{FVector(0.0f, 1.0f, 0.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)}, // Right 
	{FVector(0.0f, -1.0f, 0.0f), FProcMeshTangent(1.0, 0.0, 0.0)}, // Left
	{FVector(0.0f, 0.0f, -1.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)}, //Bottom
	{FVector(0.0f, 0.0f, 1.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)} //Top
};

void FVoxelMeshContainer::AddVirtualFaceToMesh(const FVirtualVoxelFace& Face, const EFaceDirection FaceIndex,
                                               const double VoxelSize, int MaxNumberVoxels)
{
	const auto& Voxel = Face.Voxel;
	// TODO: remove
	check(Voxel.VoxelId != 0);

	const TSharedPtr<FProcMeshSectionVars>* ProcMeshVars = VoxelTable.Find(Voxel);

	if (!ProcMeshVars)
	{
		auto NewSection = MakeShared<FProcMeshSectionVars>(MaxNumberVoxels, VoxelTable.Num());
		ProcMeshVars = &VoxelTable.Add(Voxel, NewSection);
	}

	auto& QuadSection = **ProcMeshVars;
	auto [Normal, Tangent] = FaceNormalsAndTangents[FaceIndex];
	auto& TriangleIndex = QuadSection.GlobalTriangleIndex;

	// Create quad from 2 triangles
	QuadSection.Vertices.Push(Face.GetFinalStartVertexDown(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalEndVertexDown(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalEndVertexUp(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalStartVertexUp(VoxelSize));

	QuadSection.Triangles.Push(TriangleIndex);
	QuadSection.Triangles.Push(TriangleIndex + 1);
	QuadSection.Triangles.Push(TriangleIndex + 2);
	QuadSection.Triangles.Push(TriangleIndex + 2);
	QuadSection.Triangles.Push(TriangleIndex + 3);
	QuadSection.Triangles.Push(TriangleIndex);

	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);

	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);

	QuadSection.UV0.Push(FVector2D(0, 0));
	QuadSection.UV0.Push(FVector2D(1, 0));
	QuadSection.UV0.Push(FVector2D(1, 1));
	QuadSection.UV0.Push(FVector2D(0, 1));

	TriangleIndex += 4;
}
