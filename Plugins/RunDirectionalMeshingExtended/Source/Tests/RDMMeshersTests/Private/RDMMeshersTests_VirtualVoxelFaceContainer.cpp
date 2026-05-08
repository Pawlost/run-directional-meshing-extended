#include "VoxelMesher/MeshingUtil/VirtualVoxelFaceContext.h"
#include "VoxelMesher/MeshingUtil/VoxelMesh.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsFaceWithZeroLength,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.AddsFaceWithZeroLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsFaceWithZeroLength::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 0;
	const EFaceDirection FaceIndex = EFaceDirection::Front;

	// Act
	Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices should be at position", Section->Vertices[0], FVector(0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsFaceForAllDirections,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.AddsFaceForAllDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsFaceForAllDirections::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		EFaceDirection FaceIndex = static_cast<EFaceDirection>(f);
		Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 36", Section->Triangles.Num(), 36); // 6 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsMultipleFacesSameDirection,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.AddsMultipleFacesSameDirection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AddsMultipleFacesSameDirection::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const int Length = 1;
	const EFaceDirection FaceIndex = EFaceDirection::Front;

	// Act
	for (int i = 0; i < 5; i++)
	{
		FIntVector Position(i, 0, 0);
		Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4); // Merged into one face

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_HandlesNonEmptyActiveArray,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.HandlesNonEmptyActiveArray",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_HandlesNonEmptyActiveArray::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, Position, Length);
	ActiveArray.Add(Face);
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesLargeVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.ProcessesLargeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesLargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 100.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("First vertex scaled", Section->Vertices[0], FVector(0, 1, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesZeroMaxVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.ProcessesZeroMaxVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesZeroMaxVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 0;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_Constructor_WithDifferentVoxelPlanes,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.Constructor.WithDifferentVoxelPlanes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_Constructor_WithDifferentVoxelPlanes::RunTest(const FString& Parameters)
{
	// Arrange
	const uint32 VoxelPlane1 = 1;
	const uint32 VoxelPlane2 = 1000;

	// Act
	FVirtualVoxelFaceContext Context1(VoxelPlane1);
	FVirtualVoxelFaceContext Context2(VoxelPlane2);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context1.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context2.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh1, Mesh2;
	TArray<FVirtualVoxelFace> ActiveArray1, ActiveArray2;
	TArray<FVirtualVoxelFace> PassiveArray1, PassiveArray2;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context1.DirectionalGreedyMergeForVoxelPlane(&ActiveArray1, &PassiveArray1, Mesh1, VoxelSize, MaxVoxelsInChunk);
	Context2.DirectionalGreedyMergeForVoxelPlane(&ActiveArray2, &PassiveArray2, Mesh2, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("Mesh1 should contain one entry", Mesh1.VoxelTable.Num(), 1);
	TestEqual("Mesh2 should contain one entry", Mesh2.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MultipleVoxelsWithFaceMerging,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.MultipleVoxelsWithFaceMerging",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MultipleVoxelsWithFaceMerging::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel1, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel2, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain two entries", Mesh.VoxelTable.Num(), 2);
	TestTrue("Should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("Should contain Voxel2", Mesh.VoxelTable.Contains(Voxel2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_WithNegativePositions,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.WithNegativePositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_WithNegativePositions::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(-10, -20, -30);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices should be placed correctly", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithFractionalVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.WithFractionalVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithFractionalVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 0.5;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MixedDirectionsWithMerge,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.MixedDirectionsWithMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MixedDirectionsWithMerge::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 18", Section->Triangles.Num(), 18); // 3 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_WithLargeRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.WithLargeRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_WithLargeRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 100;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithPassiveArray,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.WithPassiveArray",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithPassiveArray::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, Position, Length);
	PassiveArray.Add(Face);
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesLargeVoxelPlane,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.ProcessesLargeVoxelPlane",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_ProcessesLargeVoxelPlane::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10000);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithNegativeVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.WithNegativeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithNegativeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = -1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AllSixDirectionsWithRun,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.AllSixDirectionsWithRun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AllSixDirectionsWithRun::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(20);
	const FVoxel Voxel(1);
	const FIntVector Position(5, 5, 5);
	const int Length = 5;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Bottom, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 36", Section->Triangles.Num(), 36); // 6 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_VerySmallVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.VerySmallVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_VerySmallVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 0.001;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_SequentialFacesWithIncreasingRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.SequentialFacesWithIncreasingRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_SequentialFacesWithIncreasingRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(50);
	const FVoxel Voxel(1);
	const EFaceDirection FaceIndex = EFaceDirection::Front;

	// Act
	for (int i = 0; i < 5; i++)
	{
		FIntVector Position(i * 10, 0, 0);
		Context.AddNewVirtualFace(FaceIndex, Voxel, Position, i + 1);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_Constructor_MinimalVoxelPlane,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.Constructor.MinimalVoxelPlane",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_Constructor_MinimalVoxelPlane::RunTest(const FString& Parameters)
{
	// Arrange
	const uint32 VoxelPlane = 1;

	// Act
	FVirtualVoxelFaceContext Context(VoxelPlane);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MixedPositiveNegativeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.MixedPositiveNegativeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MixedPositiveNegativeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(-5, 10, -15);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_LargeMaxVoxelsInChunk,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.LargeMaxVoxelsInChunk",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_LargeMaxVoxelsInChunk::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 1000000;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_ContinuousFaceAddition,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.ContinuousFaceAddition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_ContinuousFaceAddition::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(100);
	const FVoxel Voxel(1);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const int Length = 1;

	// Act
	for (int i = 0; i < 10; i++)
	{
		FIntVector Position(i, 0, 0);
		Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 6", Section->Triangles.Num(), 6); // Merged into one face

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AlternatingDirections,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.AlternatingDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_AlternatingDirections::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(1, 0, 0), Length);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, FIntVector(1, 0, 0), Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_StandardVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.StandardVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_StandardVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_ExtremeLargeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.ExtremeLargeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_ExtremeLargeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(10000, 20000, 30000);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 4", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_EmptyContextThenMerge,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.EmptyContextThenMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_EmptyContextThenMerge::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should be empty", Mesh.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_DifferentVoxelsMultipleFaces,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.DifferentVoxelsMultipleFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_DifferentVoxelsMultipleFaces::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	const FVoxel Voxel3(3);

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel1, FIntVector(0, 0, 0), 1);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel2, FIntVector(1, 0, 0), 1);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel3, FIntVector(0, 1, 0), 1);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain three entries", Mesh.VoxelTable.Num(), 3);
	TestTrue("Should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("Should contain Voxel2", Mesh.VoxelTable.Contains(Voxel2));
	TestTrue("Should contain Voxel3", Mesh.VoxelTable.Contains(Voxel3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithZeroVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.WithZeroVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithZeroVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 0.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_NonContiguousFacesWithGaps,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.NonContiguousFacesWithGaps",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_NonContiguousFacesWithGaps::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(100);
	const FVoxel Voxel(1);
	const EFaceDirection FaceIndex = EFaceDirection::Front;
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(0, 0, 0), Length);
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(5, 0, 0), Length);
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(10, 0, 0), Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_BackAndFrontFacesSameVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.BackAndFrontFacesSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_BackAndFrontFacesSameVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 12", Section->Triangles.Num(), 12); // 2 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithBothActiveAndPassiveArrays,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.WithBothActiveAndPassiveArrays",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_WithBothActiveAndPassiveArrays::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	const FVirtualVoxelFace ActiveFace = FVirtualVoxelFace::CreateFrontFace(Voxel1, Position, Length);
	const FVirtualVoxelFace PassiveFace = FVirtualVoxelFace::CreateFrontFace(Voxel2, Position, Length);
	ActiveArray.Add(ActiveFace);
	PassiveArray.Add(PassiveFace);
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain two entries", Mesh.VoxelTable.Num(), 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_LeftAndRightFacesSamePosition,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.LeftAndRightFacesSamePosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_LeftAndRightFacesSamePosition::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(5, 5, 5);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 12", Section->Triangles.Num(), 12); // 2 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MultipleRunsWithinSameFace,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.MultipleRunsWithinSameFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_MultipleRunsWithinSameFace::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(50);
	const FVoxel Voxel(1);
	const EFaceDirection FaceIndex = EFaceDirection::Front;

	// Act
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(0, 0, 0), 3);
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(3, 0, 0), 2);
	Context.AddNewVirtualFace(FaceIndex, Voxel, FIntVector(5, 0, 0), 4);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_MinimalMaxVoxelsInChunk,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.MinimalMaxVoxelsInChunk",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_MinimalMaxVoxelsInChunk::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 1;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_OriginPositionAllDirections,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.OriginPositionAllDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_OriginPositionAllDirections::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(20);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		EFaceDirection FaceIndex = static_cast<EFaceDirection>(f);
		Context.AddNewVirtualFace(FaceIndex, Voxel, Position, Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Vertices count should be 8", Section->Vertices.Num(), 8); // Cube vertices

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_DoubleLargeVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.DirectionalGreedyMergeForVoxelPlane.DoubleLargeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_DirectionalGreedyMergeForVoxelPlane_DoubleLargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 2.5;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_BottomTopFacesCombined,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddNewVirtualFace.BottomTopFacesCombined",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddNewVirtualFace_BottomTopFacesCombined::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(-10, -10, -10);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Bottom, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 12", Section->Triangles.Num(), 12); // 2 faces * 6 triangles

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_RepeatedMergeOperationsPreservesData,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.RepeatedMergeOperationsPreservesData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_RepeatedMergeOperationsPreservesData::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh1, Mesh2;
	TArray<FVirtualVoxelFace> ActiveArray1, ActiveArray2;
	TArray<FVirtualVoxelFace> PassiveArray1, PassiveArray2;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray1, &PassiveArray1, Mesh1, VoxelSize, MaxVoxelsInChunk);
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray2, &PassiveArray2, Mesh2, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("First mesh should have one entry", Mesh1.VoxelTable.Num(), 1);
	TestEqual("Second mesh should have one entry", Mesh2.VoxelTable.Num(), 1);
	const auto& Section1 = Mesh1.VoxelTable[Voxel];
	const auto& Section2 = Mesh2.VoxelTable[Voxel];
	TestEqual("Both meshes should have same vertex count", Section1->Vertices.Num(), Section2->Vertices.Num());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_MergeWithMaxVoxelsLimitBoundary,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.MergeWithMaxVoxelsLimitBoundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_MergeWithMaxVoxelsLimitBoundary::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	for (int i = 0; i < 10; i++)
	{
		Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(i, 0, 0), 1);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 10;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should have vertices", Section->Vertices.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_OppositeDirectionFacesMerge,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.OppositeDirectionFacesMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_OppositeDirectionFacesMerge::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, FIntVector(1, 0, 0), Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should have geometry", Section->Vertices.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_MultipleContextsIndependent,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.MultipleContextsIndependent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_MultipleContextsIndependent::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context1(10);
	FVirtualVoxelFaceContext Context2(10);
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);

	// Act
	Context1.AddNewVirtualFace(EFaceDirection::Front, Voxel1, FIntVector(0, 0, 0), 1);
	Context2.AddNewVirtualFace(EFaceDirection::Front, Voxel2, FIntVector(5, 5, 5), 2);
	FVoxelMesh Mesh1, Mesh2;
	TArray<FVirtualVoxelFace> ActiveArray1, ActiveArray2;
	TArray<FVirtualVoxelFace> PassiveArray1, PassiveArray2;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context1.DirectionalGreedyMergeForVoxelPlane(&ActiveArray1, &PassiveArray1, Mesh1, VoxelSize, MaxVoxelsInChunk);
	Context2.DirectionalGreedyMergeForVoxelPlane(&ActiveArray2, &PassiveArray2, Mesh2, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("Mesh1 should contain Voxel1", Mesh1.VoxelTable.Num(), 1);
	TestEqual("Mesh2 should contain Voxel2", Mesh2.VoxelTable.Num(), 1);
	TestTrue("Mesh1 should have Voxel1", Mesh1.VoxelTable.Contains(Voxel1));
	TestTrue("Mesh2 should have Voxel2", Mesh2.VoxelTable.Contains(Voxel2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_VoxelSizeAffectsVertexPositions,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.VoxelSizeAffectsVertexPositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_VoxelSizeAffectsVertexPositions::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context1(10);
	FVirtualVoxelFaceContext Context2(10);
	const FVoxel Voxel(1);
	const FIntVector Position(1, 1, 1);
	const int Length = 1;

	// Act
	Context1.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context2.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh1, Mesh2;
	TArray<FVirtualVoxelFace> ActiveArray1, ActiveArray2;
	TArray<FVirtualVoxelFace> PassiveArray1, PassiveArray2;
	const int MaxVoxelsInChunk = 100;
	Context1.DirectionalGreedyMergeForVoxelPlane(&ActiveArray1, &PassiveArray1, Mesh1, 1.0, MaxVoxelsInChunk);
	Context2.DirectionalGreedyMergeForVoxelPlane(&ActiveArray2, &PassiveArray2, Mesh2, 2.0, MaxVoxelsInChunk);

	// Assert
	TestEqual("Mesh1 should have one entry", Mesh1.VoxelTable.Num(), 1);
	TestEqual("Mesh2 should have one entry", Mesh2.VoxelTable.Num(), 1);
	const auto& Section1 = Mesh1.VoxelTable[Voxel];
	const auto& Section2 = Mesh2.VoxelTable[Voxel];
	TestNotEqual("Vertices should differ with different voxel sizes", Section1->Vertices[0], Section2->Vertices[0]);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_FaceWithZeroPositionCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.FaceWithZeroPositionCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_FaceWithZeroPositionCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 5;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Should have vertices", Section->Vertices.Num(), 4);
	TestEqual("First vertex should start at origin", Section->Vertices[0], FVector(0, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AddFacesAfterMerge,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AddFacesAfterMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AddFacesAfterMerge::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(0, 0, 0), 1);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Act - Add more faces after merge
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, FIntVector(0, 0, 0), 1);
	FVoxelMesh Mesh2;
	TArray<FVirtualVoxelFace> ActiveArray2;
	TArray<FVirtualVoxelFace> PassiveArray2;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray2, &PassiveArray2, Mesh2, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("First mesh should have one entry", Mesh.VoxelTable.Num(), 1);
	TestEqual("Second mesh should have one entry", Mesh2.VoxelTable.Num(), 1);
	const auto& Section2 = Mesh2.VoxelTable[Voxel];
	TestTrue("Second mesh should have more triangles", Section2->Triangles.Num() > 6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_ThreeOrthogonalDirectionsFacesMerge,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.ThreeOrthogonalDirectionsFacesMerge",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_ThreeOrthogonalDirectionsFacesMerge::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(3, 3, 3);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Triangles count should be 18", Section->Triangles.Num(), 18);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_LargeRunLengthExceedsMaxVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.LargeRunLengthExceedsMaxVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_LargeRunLengthExceedsMaxVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(0, 0, 0);
	const int Length = 200;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 50;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should generate triangles despite exceeding max", Section->Triangles.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_ActiveArrayProcessingWithoutFaces,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.ActiveArrayProcessingWithoutFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_ActiveArrayProcessingWithoutFaces::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act - Empty arrays
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should be empty with no arrays", Mesh.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_PassiveArrayWithMultipleVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.PassiveArrayWithMultipleVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_PassiveArrayWithMultipleVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	const FVoxel Voxel3(3);
	PassiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel1, FIntVector(0, 0, 0), 1));
	PassiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel2, FIntVector(1, 0, 0), 1));
	PassiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel3, FIntVector(2, 0, 0), 1));
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain three entries", Mesh.VoxelTable.Num(), 3);
	TestTrue("Should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("Should contain Voxel2", Mesh.VoxelTable.Contains(Voxel2));
	TestTrue("Should contain Voxel3", Mesh.VoxelTable.Contains(Voxel3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_FractionalVoxelSizeWithLargeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.FractionalVoxelSizeWithLargeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_FractionalVoxelSizeWithLargeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector Position(1000, 2000, 3000);
	const int Length = 1;
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 0.25;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Should have 4 vertices", Section->Vertices.Num(), 4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_ConsecutiveFacesFormsContinuousStrip,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.ConsecutiveFacesFormsContinuousStrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_ConsecutiveFacesFormsContinuousStrip::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(20);
	const FVoxel Voxel(1);
	const EFaceDirection Direction = EFaceDirection::Front;
	const int Length = 1;

	// Act - Add continuous strip of faces
	for (int i = 0; i < 7; i++)
	{
		Context.AddNewVirtualFace(Direction, Voxel, FIntVector(i, 0, 0), Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Should have exactly 4 vertices after merge", Section->Vertices.Num(), 4);
	TestEqual("Should have 6 triangles", Section->Triangles.Num(), 6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_MaxVoxelPlaneWithSingleFace,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.MaxVoxelPlaneWithSingleFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_MaxVoxelPlaneWithSingleFace::RunTest(const FString& Parameters)
{
	// Arrange
	const uint32 MaxVoxelPlane = 999999;
	FVirtualVoxelFaceContext Context(MaxVoxelPlane);
	const FVoxel Voxel(1);
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(MaxVoxelPlane - 1, 0, 0), 1);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_SymmetricalFaceAddition,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.SymmetricalFaceAddition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_SymmetricalFaceAddition::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel(1);
	const FIntVector OriginPos(0, 0, 0);
	const int Length = 1;

	// Act - Add symmetrically opposite faces
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, OriginPos, Length);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, OriginPos, Length);
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, OriginPos, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, OriginPos, Length);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, OriginPos, Length);
	Context.AddNewVirtualFace(EFaceDirection::Bottom, Voxel, OriginPos, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Should form complete cube with 8 unique vertices", Section->Vertices.Num(), 8);
	TestEqual("Should have 36 triangles for complete cube", Section->Triangles.Num(), 36);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_MixedSingleAndMultiLengthFaces,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.MixedSingleAndMultiLengthFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_MixedSingleAndMultiLengthFaces::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(50);
	const FVoxel Voxel(1);
	const EFaceDirection Direction = EFaceDirection::Front;

	// Act - Mix single and multi-length runs
	Context.AddNewVirtualFace(Direction, Voxel, FIntVector(0, 0, 0), 1);
	Context.AddNewVirtualFace(Direction, Voxel, FIntVector(1, 0, 0), 5);
	Context.AddNewVirtualFace(Direction, Voxel, FIntVector(6, 0, 0), 1);
	Context.AddNewVirtualFace(Direction, Voxel, FIntVector(7, 0, 0), 3);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should generate geometry", Section->Vertices.Num() > 0);
	TestTrue("Should generate triangles", Section->Triangles.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_VoxelIdentitieDifferentiation,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.VoxelIdentitieDifferentiation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_VoxelIdentitieDifferentiation::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(1);
	const FVoxel Voxel3(2);
	const FIntVector Position(0, 0, 0);
	const int Length = 1;

	// Act
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel1, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel2, Position, Length);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel3, Position, Length);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should distinguish different voxel IDs", Mesh.VoxelTable.Num(), 2);
	TestTrue("Should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("Should contain Voxel3", Mesh.VoxelTable.Contains(Voxel3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AlternatingDirectionSequence,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AlternatingDirectionSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AlternatingDirectionSequence::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(20);
	const FVoxel Voxel(1);
	const int Length = 1;

	// Act - Alternate between two directions repeatedly
	for (int i = 0; i < 4; i++)
	{
		Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(i, 0, 0), Length);
		Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, FIntVector(i, 0, 0), Length);
	}
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should generate geometry", Section->Vertices.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_ActiveArrayWithMixedVoxelTypes,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.ActiveArrayWithMixedVoxelTypes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_ActiveArrayWithMixedVoxelTypes::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(10);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(100);
	const FVoxel Voxel3(999);
	ActiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel1, FIntVector(0, 0, 0), 1));
	ActiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel2, FIntVector(1, 0, 0), 1));
	ActiveArray.Add(FVirtualVoxelFace::CreateFrontFace(Voxel3, FIntVector(2, 0, 0), 1));
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain three voxel types", Mesh.VoxelTable.Num(), 3);
	TestTrue("Should contain Voxel1", Mesh.VoxelTable.Contains(Voxel1));
	TestTrue("Should contain Voxel2", Mesh.VoxelTable.Contains(Voxel2));
	TestTrue("Should contain Voxel3", Mesh.VoxelTable.Contains(Voxel3));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_VeryLargeVoxelPlaneSize,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.VeryLargeVoxelPlaneSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_VeryLargeVoxelPlaneSize::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(1000000);
	const FVoxel Voxel(1);
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(500000, 0, 0), 1);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;

	// Act
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestTrue("Should have valid geometry", Section->Vertices.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFaceContext_AllDirectionsCombinedWithVariableLengths,
	"RDM.RDMMeshersTests.VirtualVoxelFaceContext.AllDirectionsCombinedWithVariableLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFaceContext_AllDirectionsCombinedWithVariableLengths::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFaceContext Context(50);
	const FVoxel Voxel(1);

	// Act - Add all six directions with different lengths
	Context.AddNewVirtualFace(EFaceDirection::Front, Voxel, FIntVector(0, 0, 0), 1);
	Context.AddNewVirtualFace(EFaceDirection::Back, Voxel, FIntVector(0, 0, 0), 2);
	Context.AddNewVirtualFace(EFaceDirection::Left, Voxel, FIntVector(0, 0, 0), 3);
	Context.AddNewVirtualFace(EFaceDirection::Right, Voxel, FIntVector(0, 0, 0), 1);
	Context.AddNewVirtualFace(EFaceDirection::Top, Voxel, FIntVector(0, 0, 0), 4);
	Context.AddNewVirtualFace(EFaceDirection::Bottom, Voxel, FIntVector(0, 0, 0), 2);
	FVoxelMesh Mesh;
	TArray<FVirtualVoxelFace> ActiveArray;
	TArray<FVirtualVoxelFace> PassiveArray;
	const double VoxelSize = 1.0;
	const int MaxVoxelsInChunk = 100;
	Context.DirectionalGreedyMergeForVoxelPlane(&ActiveArray, &PassiveArray, Mesh, VoxelSize, MaxVoxelsInChunk);

	// Assert
	TestEqual("VoxelTable should contain one entry", Mesh.VoxelTable.Num(), 1);
	const auto& Section = Mesh.VoxelTable[Voxel];
	TestEqual("Should have 36 triangles for complete cube", Section->Triangles.Num(), 36);

	return true;
}
