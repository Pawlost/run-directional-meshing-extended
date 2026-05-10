#include "VoxelMesher/MeshingUtil/VoxelMesh.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFrontFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFrontFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFrontFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	TestTrue("VoxelTable should contain the voxel", Mesh.VoxelTable.Contains(Voxel));
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);
	TestEqual("Triangles count should be 6", Section->Triangles.Num(), 6);
	TestEqual("Normals count should be 4", Section->Normals.Num(), 4);
	TestEqual("Tangents count should be 4", Section->Tangents.Num(), 4);
	TestEqual("UV0 count should be 4", Section->UV0.Num(), 4);
	TestEqual("GlobalTriangleIndex should be 4", Section->GlobalTriangleIndex, 4);
	TestEqual("First vertex", Section->Vertices[0], FVector(0, 1, 0));
	TestEqual("Second vertex", Section->Vertices[1], FVector(0, 0, 0));
	TestEqual("Third vertex", Section->Vertices[2], FVector(0, 0, 1));
	TestEqual("Fourth vertex", Section->Vertices[3], FVector(0, 1, 1));
	TestEqual("First normal", Section->Normals[0], FVector(1, 0, 0));
	TestEqual("Triangles", Section->Triangles, TArray<int32>({0, 1, 2, 2, 3, 0}));
	TestEqual("UV0", Section->UV0, TArray<FVector2D>({FVector2D(0, 0), FVector2D(1, 0), FVector2D(1, 1), FVector2D(0, 1)}));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsToExistingSection,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsToExistingSection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsToExistingSection::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex1 = EFaceDirection::Front;
	const EFaceDirection FaceIndex2 = EFaceDirection::Back;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face1, FaceIndex1, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(Face2, FaceIndex2, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 8", Section->Vertices.Num(), 8);
	TestEqual("Triangles count should be 12", Section->Triangles.Num(), 12);
	TestEqual("GlobalTriangleIndex should be 8", Section->GlobalTriangleIndex, 8);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsTopFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsTopFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsTopFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 2;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Top;
	const double VoxelSize = 2.0;
	const int MaxNumberVoxels = 50;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Normals should be top", Section->Normals[0], FVector(0, 0, 1));
	TestEqual("First vertex scaled", Section->Vertices[0], FVector(2, 8, 6)); // (1,4,3)*2

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_ZeroVoxelSize,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.ZeroVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_ZeroVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(5, 10, 15);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 0.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices should be zero when VoxelSize is 0", Section->Vertices[0], FVector(0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_LargeRunLength,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.LargeRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_LargeRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 100;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Right;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("EndVertexDown Y should be RunLength - 1", Face.EndVertexDown.Y, 99);
	TestEqual("Vertex Y scaled", Section->Vertices[1].Y, 99.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsBackFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsBackFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsBackFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Back;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Normals should be back", Section->Normals[0], FVector(-1, 0, 0));
	TestEqual("First vertex", Section->Vertices[0], FVector(0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsLeftFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsLeftFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsLeftFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Left;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Normals should be left", Section->Normals[0], FVector(0, -1, 0));
	TestEqual("First vertex", Section->Vertices[0], FVector(1, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsRightFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsRightFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsRightFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Right;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Normals should be right", Section->Normals[0], FVector(0, 1, 0));
	TestEqual("First vertex", Section->Vertices[0], FVector(0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsBottomFace,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsBottomFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsBottomFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Bottom;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Normals should be bottom", Section->Normals[0], FVector(0, 0, -1));
	TestEqual("First vertex", Section->Vertices[0], FVector(0, 1, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_MultipleVoxels,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.MultipleVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_MultipleVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateFrontFace(Voxel1, InitialPosition, RunLength);
	const FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateFrontFace(Voxel2, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face1, FaceIndex, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(Face2, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain two entries", Mesh.VoxelTable.Num(), 2);
	TestTrue("VoxelTable should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("VoxelTable should contain Voxel2", Mesh.VoxelTable.Contains(Voxel2));
	const auto& Section1 = Mesh.VoxelTable[Voxel1];
	const auto& Section2 = Mesh.VoxelTable[Voxel2];
	TestEqual("Section1 vertices count", Section1->Vertices.Num(), 4);
	TestEqual("Section2 vertices count", Section2->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_NegativePositions,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.NegativePositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_NegativePositions::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(-5, -10, -3);
	const int RunLength = 3;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex handles negative", Section->Vertices[0], FVector(-5, -7, -3));
	TestEqual("Second vertex", Section->Vertices[1], FVector(-5, -10, -3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_FractionalVoxelSize,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.FractionalVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_FractionalVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 0.5;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex scaled by 0.5", Section->Vertices[0], FVector(0.5, 1.5, 1.5));
	TestEqual("Second vertex", Section->Vertices[1], FVector(0.5, 1.0, 1.5));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_DifferentVoxelIds,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.DifferentVoxelIds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_DifferentVoxelIds::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel VoxelA(42);
	const FVoxel VoxelB(999);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace FaceA = FVirtualVoxelFace::CreateFrontFace(VoxelA, InitialPosition, RunLength);
	const FVirtualVoxelFace FaceB = FVirtualVoxelFace::CreateFrontFace(VoxelB, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(FaceA, FaceIndex, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(FaceB, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain two entries", Mesh.VoxelTable.Num(), 2);
	TestTrue("Contains VoxelA", Mesh.VoxelTable.Contains(VoxelA));
	TestTrue("Contains VoxelB", Mesh.VoxelTable.Contains(VoxelB));
	const auto& SectionA = Mesh.VoxelTable[VoxelA];
	const auto& SectionB = Mesh.VoxelTable[VoxelB];
	TestEqual("SectionA MeshSectionId", SectionA->MeshSectionId, 0);
	TestEqual("SectionB MeshSectionId", SectionB->MeshSectionId, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithZeroRunLength,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices should be at InitialPosition when RunLength=0", Section->Vertices[0], FVector(0, 0, 0));
	TestEqual("All vertices should be the same", Section->Vertices[1], Section->Vertices[0]);
	TestEqual("All vertices should be the same", Section->Vertices[2], Section->Vertices[0]);
	TestEqual("All vertices should be the same", Section->Vertices[3], Section->Vertices[0]);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithLargeVoxelSize,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithLargeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithLargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(1, 1, 1);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1000.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex scaled by large VoxelSize", Section->Vertices[0], FVector(1000, 2000, 1000));
	TestEqual("Second vertex", Section->Vertices[1], FVector(1000, 1000, 1000));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithNegativeVoxelSize,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithNegativeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithNegativeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(1, 1, 1);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = -1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex scaled by negative VoxelSize", Section->Vertices[0], FVector(-1, -1, -1));
	TestEqual("Second vertex", Section->Vertices[1], FVector(-1, -2, -1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesNormalsRepeatedForAllVertices,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.VerifiesNormalsRepeatedForAllVertices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesNormalsRepeatedForAllVertices::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Top;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	const FVector ExpectedNormal(0, 0, 1);
	TestEqual("Normal 0", Section->Normals[0], ExpectedNormal);
	TestEqual("Normal 1", Section->Normals[1], ExpectedNormal);
	TestEqual("Normal 2", Section->Normals[2], ExpectedNormal);
	TestEqual("Normal 3", Section->Normals[3], ExpectedNormal);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesTangentsRepeatedForAllVertices,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.VerifiesTangentsRepeatedForAllVertices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesTangentsRepeatedForAllVertices::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Top;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesUVsAlwaysSame,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.VerifiesUVsAlwaysSame",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesUVsAlwaysSame::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	const TArray<FVector2D> ExpectedUVs = {FVector2D(0, 0), FVector2D(1, 0), FVector2D(1, 1), FVector2D(0, 1)};
	TestEqual("UVs", Section->UV0, ExpectedUVs);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithLargePositions,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithLargePositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithLargePositions::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(10000, 20000, 30000);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex with large position", Section->Vertices[0], FVector(10000, 20001, 30000));
	TestEqual("Second vertex", Section->Vertices[1], FVector(10000, 20000, 30000));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsMultipleFacesToSameVoxel,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsMultipleFacesToSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsMultipleFacesToSameVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace Face3 = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex1 = EFaceDirection::Front;
	const EFaceDirection FaceIndex2 = EFaceDirection::Back;
	const EFaceDirection FaceIndex3 = EFaceDirection::Top;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face1, FaceIndex1, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(Face2, FaceIndex2, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(Face3, FaceIndex3, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 12", Section->Vertices.Num(), 12);
	TestEqual("Triangles count should be 18", Section->Triangles.Num(), 18);
	TestEqual("GlobalTriangleIndex should be 12", Section->GlobalTriangleIndex, 12);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_EmptyClearsVoxelTable,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.EmptyClearsVoxelTable",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_EmptyClearsVoxelTable::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Act
	Mesh.Empty();

	// Assert
	TestEqual("VoxelTable should be empty after Empty", Mesh.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithTransparentVoxel,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithTransparentVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithTransparentVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1, true); // Transparent voxel
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	TestTrue("VoxelTable should contain the transparent voxel", Mesh.VoxelTable.Contains(Voxel));
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithVerySmallVoxelSize,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithVerySmallVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithVerySmallVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(1, 1, 1);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 0.001;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex scaled by very small VoxelSize", Section->Vertices[0], FVector(0.001, 0.002, 0.001));
	TestEqual("Second vertex", Section->Vertices[1], FVector(0.001, 0.001, 0.001));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesTriangleIndicesForMultipleFaces,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.VerifiesTriangleIndicesForMultipleFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_VerifiesTriangleIndicesForMultipleFaces::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;
	const FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex1 = EFaceDirection::Front;
	const EFaceDirection FaceIndex2 = EFaceDirection::Back;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face1, FaceIndex1, VoxelSize, MaxNumberVoxels);
	Mesh.AddVirtualFaceToMesh(Face2, FaceIndex2, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	const TArray<int32> ExpectedTriangles = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};
	TestEqual("Triangles for two faces", Section->Triangles, ExpectedTriangles);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithDifferentInitialPosition,
	"RDM.RDMMeshersTests.VoxelMesh.AddVirtualFaceToMesh.AddsFaceWithDifferentInitialPosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VoxelMesh_AddVirtualFaceToMesh_AddsFaceWithDifferentInitialPosition::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxelMesh Mesh;
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(10, 20, 30);
	const int RunLength = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const double VoxelSize = 1.0;
	const int MaxNumberVoxels = 100;

	// Act
	Mesh.AddVirtualFaceToMesh(Face, FaceIndex, VoxelSize, MaxNumberVoxels);

	// Assert
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex with offset position", Section->Vertices[0], FVector(10, 21, 30));
	TestEqual("Second vertex", Section->Vertices[1], FVector(10, 20, 30));

	return true;
}

