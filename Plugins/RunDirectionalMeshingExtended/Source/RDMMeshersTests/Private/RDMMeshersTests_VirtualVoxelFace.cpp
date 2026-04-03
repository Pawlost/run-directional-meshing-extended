#include "Misc/AutomationTest.h"
#include "VoxelMesher/MeshingUtil/VirtualVoxelFace.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_DefaultStartVertexDownIsZero,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Default.StartVertexDownIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_DefaultStartVertexDownIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace NewVirtualFace = FVirtualVoxelFace();
	// Act
	const FIntVector Actual = NewVirtualFace.StartVertexDown;
	// Assert
	TestTrue(TEXT("StartVertexDown is zero by default"), Actual == FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_DefaultEndVertexDownIsZero,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Default.EndVertexDownIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_DefaultEndVertexDownIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace NewVirtualFace = FVirtualVoxelFace();
	// Act
	const FIntVector Actual = NewVirtualFace.EndVertexDown;
	// Assert
	TestTrue(TEXT("EndVertexDown is zero by default"), Actual == FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_DefaultEndVertexUpIsZero,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Default.EndVertexUpIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_DefaultEndVertexUpIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace NewVirtualFace = FVirtualVoxelFace();
	// Act
	const FIntVector Actual = NewVirtualFace.EndVertexUp;
	// Assert
	TestTrue(TEXT("EndVertexUp is zero by default"), Actual == FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_DefaultStartVertexUpIsZero,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Default.StartVertexUpIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_DefaultStartVertexUpIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace NewVirtualFace = FVirtualVoxelFace();
	// Act
	const FIntVector Actual = NewVirtualFace.StartVertexUp;
	// Assert
	TestTrue(TEXT("StartVertexUp is zero by default"), Actual == FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ConstructedPreservesStartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Constructed.PreservesStartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ConstructedPreservesStartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	FIntVector B(4, 5, 6);
	FIntVector C(7, 8, 9);
	FIntVector D(10, 11, 12);
	FVoxel V; // default voxel
	FVirtualVoxelFace Face(V, A, B, C, D);
	// Act
	const FIntVector Actual = Face.StartVertexDown;
	// Assert
	TestTrue(TEXT("StartVertexDown preserved after construction"), Actual == A);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ConstructedPreservesEndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Constructed.PreservesEndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ConstructedPreservesEndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	FIntVector B(4, 5, 6);
	FIntVector C(7, 8, 9);
	FIntVector D(10, 11, 12);
	FVoxel V; // default voxel
	FVirtualVoxelFace Face(V, A, B, C, D);
	// Act
	const FIntVector Actual = Face.EndVertexDown;
	// Assert
	TestTrue(TEXT("EndVertexDown preserved after construction"), Actual == B);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ConstructedPreservesEndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Constructed.PreservesEndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ConstructedPreservesEndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	FIntVector B(4, 5, 6);
	FIntVector C(7, 8, 9);
	FIntVector D(10, 11, 12);
	FVoxel V; // default voxel
	FVirtualVoxelFace Face(V, A, B, C, D);
	// Act
	const FIntVector Actual = Face.EndVertexUp;
	// Assert
	TestTrue(TEXT("EndVertexUp preserved after construction"), Actual == C);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ConstructedPreservesStartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.Constructed.PreservesStartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ConstructedPreservesStartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	FIntVector B(4, 5, 6);
	FIntVector C(7, 8, 9);
	FIntVector D(10, 11, 12);
	FVoxel V; // default voxel
	FVirtualVoxelFace Face(V, A, B, C, D);
	// Act
	const FIntVector Actual = Face.StartVertexUp;
	// Assert
	TestTrue(TEXT("StartVertexUp preserved after construction"), Actual == D);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetXFromVector_ReturnsX,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetXFromVector.ReturnsX",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetXFromVector_ReturnsX::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Pos(-5, 7, 42);
	// Act
	int X = FVirtualVoxelFace::GetXFromVector(Pos);
	// Assert
	TestEqual(TEXT("GetXFromVector returns X component"), X, -5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetYFromVector_ReturnsY,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetYFromVector.ReturnsY",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetYFromVector_ReturnsY::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Pos(-5, 7, 42);
	// Act
	int Y = FVirtualVoxelFace::GetYFromVector(Pos);
	// Assert
	TestEqual(TEXT("GetYFromVector returns Y component"), Y, 7);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetZFromVector_ReturnsZ,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetZFromVector.ReturnsZ",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetZFromVector_ReturnsZ::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Pos(-5, 7, 42);
	// Act
	int Z = FVirtualVoxelFace::GetZFromVector(Pos);
	// Assert
	TestEqual(TEXT("GetZFromVector returns Z component"), Z, 42);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_ScalesByVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexDown.ScalesByVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_ScalesByVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	double VoxelSize = 0.5;
	const float Epsilon = 1e-6f;
	FVirtualVoxelFace Face(FVoxel(), A, FIntVector(0), FIntVector(0), FIntVector(0));
	FVector Expected((float)A.X * (float)VoxelSize, (float)A.Y * (float)VoxelSize, (float)A.Z * (float)VoxelSize);
	// Act
	FVector Actual = Face.GetFinalStartVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexDown scales components by VoxelSize"), Actual.Equals(Expected, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalEndVertexDown_ScalesByVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexDown.ScalesByVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalEndVertexDown_ScalesByVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector B(4, 5, 6);
	double VoxelSize = 0.5;
	const float Epsilon = 1e-6f;
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), B, FIntVector(0), FIntVector(0));
	FVector Expected((float)B.X * (float)VoxelSize, (float)B.Y * (float)VoxelSize, (float)B.Z * (float)VoxelSize);
	// Act
	FVector Actual = Face.GetFinalEndVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalEndVertexDown scales components by VoxelSize"), Actual.Equals(Expected, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalEndVertexUp_ScalesByVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexUp.ScalesByVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalEndVertexUp_ScalesByVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector C(7, 8, 9);
	double VoxelSize = 0.5;
	const float Epsilon = 1e-6f;
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), FIntVector(0), C, FIntVector(0));
	FVector Expected((float)C.X * (float)VoxelSize, (float)C.Y * (float)VoxelSize, (float)C.Z * (float)VoxelSize);
	// Act
	FVector Actual = Face.GetFinalEndVertexUp(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalEndVertexUp scales components by VoxelSize"), Actual.Equals(Expected, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalStartVertexUp_ScalesByVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexUp.ScalesByVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalStartVertexUp_ScalesByVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector D(10, 11, 12);
	double VoxelSize = 0.5;
	const float Epsilon = 1e-6f;
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), FIntVector(0), FIntVector(0), D);
	FVector Expected((float)D.X * (float)VoxelSize, (float)D.Y * (float)VoxelSize, (float)D.Z * (float)VoxelSize);
	// Act
	FVector Actual = Face.GetFinalStartVertexUp(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexUp scales components by VoxelSize"), Actual.Equals(Expected, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_StartVertexDownIsZeroVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.ZeroVoxelSize.StartVertexDownIsZeroVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_StartVertexDownIsZeroVector::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(1, 2, 3);
	FVirtualVoxelFace Face(FVoxel(), A, FIntVector(0), FIntVector(0), FIntVector(0));
	double VoxelSize = 0.0;
	const float Epsilon = 1e-6f;
	// Act
	FVector Actual = Face.GetFinalStartVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexDown with zero VoxelSize returns zero vector"), Actual.Equals(FVector::ZeroVector, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_EndVertexDownIsZeroVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.ZeroVoxelSize.EndVertexDownIsZeroVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_EndVertexDownIsZeroVector::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector B(4, 5, 6);
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), B, FIntVector(0), FIntVector(0));
	double VoxelSize = 0.0;
	const float Epsilon = 1e-6f;
	// Act
	FVector Actual = Face.GetFinalEndVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalEndVertexDown with zero VoxelSize returns zero vector"), Actual.Equals(FVector::ZeroVector, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_EndVertexUpIsZeroVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.ZeroVoxelSize.EndVertexUpIsZeroVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_EndVertexUpIsZeroVector::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector C(7, 8, 9);
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), FIntVector(0), C, FIntVector(0));
	double VoxelSize = 0.0;
	const float Epsilon = 1e-6f;
	// Act
	FVector Actual = Face.GetFinalEndVertexUp(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalEndVertexUp with zero VoxelSize returns zero vector"), Actual.Equals(FVector::ZeroVector, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_StartVertexUpIsZeroVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.ZeroVoxelSize.StartVertexUpIsZeroVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_ZeroVoxelSize_StartVertexUpIsZeroVector::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector D(10, 11, 12);
	FVirtualVoxelFace Face(FVoxel(), FIntVector(0), FIntVector(0), FIntVector(0), D);
	double VoxelSize = 0.0;
	const float Epsilon = 1e-6f;
	// Act
	FVector Actual = Face.GetFinalStartVertexUp(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexUp with zero VoxelSize returns zero vector"), Actual.Equals(FVector::ZeroVector, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionX_TrueWhenGreater,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionX.TrueWhenGreater",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionX_TrueWhenGreater::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexDown = FIntVector(0, 0, 5);
	NewFace.StartVertexUp = FIntVector(0, 0, 3);
	// Act
	bool Result = FVirtualVoxelFace::MergeFailConditionX(Face, NewFace);
	// Assert
	TestTrue(TEXT("MergeFailConditionX returns true when StartVertexDown.Z > New.StartVertexUp.Z"), Result);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionX_FalseWhenEqualOrLess,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionX.FalseWhenEqualOrLess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionX_FalseWhenEqualOrLess::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexDown = FIntVector(0, 0, 5);
	// Act & Assert for equal
	NewFace.StartVertexUp = FIntVector(0, 0, 5);
	TestFalse(TEXT("MergeFailConditionX returns false when StartVertexDown.Z == New.StartVertexUp.Z"), FVirtualVoxelFace::MergeFailConditionX(Face, NewFace));
	// Act & Assert for less
	NewFace.StartVertexUp = FIntVector(0, 0, 6);
	TestFalse(TEXT("MergeFailConditionX returns false when StartVertexDown.Z < New.StartVertexUp.Z"), FVirtualVoxelFace::MergeFailConditionX(Face, NewFace));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionRightY_TrueWhenGreater,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionRightY.TrueWhenGreater",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionRightY_TrueWhenGreater::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexUp = FIntVector(5, 0, 0);
	NewFace.EndVertexDown = FIntVector(3, 0, 0);
	// Act
	bool Result = FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace);
	// Assert
	TestTrue(TEXT("MergeFailConditionRightY returns true when StartVertexUp.X > New.EndVertexDown.X"), Result);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionRightY_FalseWhenEqualOrLess,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionRightY.FalseWhenEqualOrLess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionRightY_FalseWhenEqualOrLess::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexUp = FIntVector(5, 0, 0);
	// Act & Assert for equal
	NewFace.EndVertexDown = FIntVector(5, 0, 0);
	TestFalse(TEXT("MergeFailConditionRightY returns false when StartVertexUp.X == New.EndVertexDown.X"), FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace));
	// Act & Assert for less
	NewFace.EndVertexDown = FIntVector(6, 0, 0);
	TestFalse(TEXT("MergeFailConditionRightY returns false when StartVertexUp.X < New.EndVertexDown.X"), FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionLeftY_TrueWhenGreater,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionLeftY.TrueWhenGreater",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionLeftY_TrueWhenGreater::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.EndVertexUp = FIntVector(7, 0, 0);
	NewFace.StartVertexUp = FIntVector(5, 0, 0);
	// Act
	bool Result = FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace);
	// Assert
	TestTrue(TEXT("MergeFailConditionLeftY returns true when EndVertexUp.X > New.StartVertexUp.X"), Result);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionLeftY_FalseWhenEqualOrLess,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionLeftY.FalseWhenEqualOrLess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionLeftY_FalseWhenEqualOrLess::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.EndVertexUp = FIntVector(7, 0, 0);
	// Act & Assert for equal
	NewFace.StartVertexUp = FIntVector(7, 0, 0);
	TestFalse(TEXT("MergeFailConditionLeftY returns false when EndVertexUp.X == New.StartVertexUp.X"), FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace));
	// Act & Assert for less
	NewFace.StartVertexUp = FIntVector(9, 0, 0);
	TestFalse(TEXT("MergeFailConditionLeftY returns false when EndVertexUp.X < New.StartVertexUp.X"), FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionZ_TrueWhenGreater,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionZ.TrueWhenGreater",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionZ_TrueWhenGreater::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexDown = FIntVector(5, 0, 0);
	NewFace.StartVertexUp = FIntVector(3, 0, 0);
	// Act
	bool Result = FVirtualVoxelFace::MergeFailConditionZ(Face, NewFace);
	// Assert
	TestTrue(TEXT("MergeFailConditionZ returns true when StartVertexDown.X > New.StartVertexUp.X"), Result);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFailConditionZ_FalseWhenEqualOrLess,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionZ.FalseWhenEqualOrLess",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFailConditionZ_FalseWhenEqualOrLess::RunTest(const FString& Parameters)
{
	// Arrange
	FVirtualVoxelFace Face;
	FVirtualVoxelFace NewFace;
	Face.StartVertexDown = FIntVector(5, 0, 0);
	// Act & Assert for equal
	NewFace.StartVertexUp = FIntVector(5, 0, 0);
	TestFalse(TEXT("MergeFailConditionZ returns false when StartVertexDown.X == New.StartVertexUp.X"), FVirtualVoxelFace::MergeFailConditionZ(Face, NewFace));
	// Act & Assert for less
	NewFace.StartVertexUp = FIntVector(6, 0, 0);
	TestFalse(TEXT("MergeFailConditionZ returns false when StartVertexDown.X < New.StartVertexUp.X"), FVirtualVoxelFace::MergeFailConditionZ(Face, NewFace));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceEnd_MergesWhenAdjacentAndSameVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.MergesWhenAdjacentAndSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceEnd_MergesWhenAdjacentAndSameVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(1);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(0), FIntVector(1, 0, 0), FIntVector(1, 0, 1), FIntVector(0));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(1, 0, 0), FIntVector(6, 0, 1), FIntVector(1, 0, 1),  FIntVector(1, 0, 1));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceEnd(Prev, NewFace);
	// Assert
	TestTrue(TEXT("MergeFaceEnd returns true when faces are adjacent and have same voxel"), Merged);
	TestTrue(TEXT("Prev.EndVertexDown updated to New.EndVertexDown after merge"), Prev.EndVertexDown == NewFace.EndVertexDown);
	TestTrue(TEXT("Prev.EndVertexUp updated to New.EndVertexUp after merge"), Prev.EndVertexUp == NewFace.EndVertexUp);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceEnd_FailsWhenVoxelsDiffer,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.FailsWhenVoxelsDiffer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceEnd_FailsWhenVoxelsDiffer::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(1);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(0), FIntVector(1, 0, 0), FIntVector(1, 0, 1), FIntVector(0));
	FVirtualVoxelFace NewDifferent(FVoxel(2), FIntVector(1, 0, 0), FIntVector(9, 9, 9), FIntVector(1, 0, 1), FIntVector(8, 8, 8));
	// Act
	bool MergedDifferent = FVirtualVoxelFace::MergeFaceEnd(Prev, NewDifferent);
	// Assert
	TestFalse(TEXT("MergeFaceEnd returns false when voxels differ"), MergedDifferent);
	TestTrue(TEXT("Prev.EndVertexDown unchanged when merge fails"), Prev.EndVertexDown == FIntVector(1, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceStart_MergesWhenAdjacentAndSameVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.MergesWhenAdjacentAndSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceStart_MergesWhenAdjacentAndSameVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(3);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(5, 5, 5), FIntVector(6, 6, 6), FIntVector(7, 7, 7), FIntVector(8, 8, 8));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(2, 2, 2), FIntVector(3, 3, 3), FIntVector(6, 6, 6), FIntVector(5, 5, 5));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceStart(Prev, NewFace);
	// Assert
	TestTrue(TEXT("MergeFaceStart returns true when faces match at start/end and same voxel"), Merged);
	TestTrue(TEXT("Prev.StartVertexDown updated to New.StartVertexDown after merge"), Prev.StartVertexDown == NewFace.StartVertexDown);
	TestTrue(TEXT("Prev.StartVertexUp updated to New.StartVertexUp after merge"), Prev.StartVertexUp == NewFace.StartVertexUp);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceUp_MergesWhenAdjacentAndSameVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.MergesWhenAdjacentAndSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceUp_MergesWhenAdjacentAndSameVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(4);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(0), FIntVector(1, 0, 0), FIntVector(1, 0, 1), FIntVector(2, 0, 1));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(2, 0, 1), FIntVector(3, 0, 1), FIntVector(1, 0, 0), FIntVector(4, 0, 1));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceUp(Prev, NewFace);
	// Assert
	TestTrue(TEXT("MergeFaceUp returns true when faces align vertically and have same voxel"), Merged);
	TestTrue(TEXT("Prev.StartVertexUp updated to New.StartVertexUp after merge"), Prev.StartVertexUp == NewFace.StartVertexUp);
	TestTrue(TEXT("Prev.EndVertexUp updated to New.EndVertexUp after merge"), Prev.EndVertexUp == NewFace.EndVertexUp);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateFrontFace_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateFrontFace_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(10, 20, 30);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateFrontFace StartVertexDown equals Initial + (0,RunLength,0)"), Face.StartVertexDown == Init + FIntVector(0, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateFrontFace_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateFrontFace_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(10, 20, 30);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateFrontFace EndVertexDown equals Initial + (0,0,0)"), Face.EndVertexDown == Init + FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateFrontFace_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateFrontFace_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(10, 20, 30);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateFrontFace EndVertexUp equals Initial + (0,0,1)"), Face.EndVertexUp == Init + FIntVector(0, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateFrontFace_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateFrontFace_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(10, 20, 30);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateFrontFace StartVertexUp equals Initial + (0,RunLength,1)"), Face.StartVertexUp == Init + FIntVector(0, RunLength, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceEnd_DoesNotMergeWhenNotAdjacent,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.DoesNotMergeWhenNotAdjacent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceEnd_DoesNotMergeWhenNotAdjacent::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(10);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(0), FIntVector(1, 0, 0), FIntVector(1, 0, 1), FIntVector(0));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(2, 0, 0), FIntVector(5, 0, 0), FIntVector(2, 0, 1), FIntVector(6, 0, 1));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceEnd(Prev, NewFace);
	// Assert
	TestFalse(TEXT("MergeFaceEnd does not merge when vertices are not adjacent even if voxel matches"), Merged);
	TestTrue(TEXT("Prev unchanged when MergeFaceEnd fails"), Prev.EndVertexDown == FIntVector(1, 0, 0) && Prev.EndVertexUp == FIntVector(1, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceStart_DoesNotMergeWhenNotAdjacent,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.DoesNotMergeWhenNotAdjacent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceStart_DoesNotMergeWhenNotAdjacent::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(11);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(5, 5, 5), FIntVector(6, 6, 6), FIntVector(7, 7, 7), FIntVector(8, 8, 8));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(0, 0, 0), FIntVector(1, 1, 1), FIntVector(2, 2, 2), FIntVector(3, 3, 3));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceStart(Prev, NewFace);
	// Assert
	TestFalse(TEXT("MergeFaceStart does not merge when matching vertices are not equal"), Merged);
	TestTrue(TEXT("Prev unchanged when MergeFaceStart fails"), Prev.StartVertexDown == FIntVector(5, 5, 5) && Prev.StartVertexUp == FIntVector(8, 8, 8));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_MergeFaceUp_DoesNotMergeWhenNotAdjacent,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.DoesNotMergeWhenNotAdjacent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_MergeFaceUp_DoesNotMergeWhenNotAdjacent::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel CommonVoxel(12);
	FVirtualVoxelFace Prev(CommonVoxel, FIntVector(0), FIntVector(1, 0, 0), FIntVector(1, 0, 1), FIntVector(2, 0, 1));
	FVirtualVoxelFace NewFace(CommonVoxel, FIntVector(9, 9, 9), FIntVector(10, 10, 10), FIntVector(11, 11, 11), FIntVector(12, 12, 12));
	// Act
	bool Merged = FVirtualVoxelFace::MergeFaceUp(Prev, NewFace);
	// Assert
	TestFalse(TEXT("MergeFaceUp does not merge when vertical adjacency does not hold"), Merged);
	TestTrue(TEXT("Prev unchanged when MergeFaceUp fails"), Prev.StartVertexUp == FIntVector(2, 0, 1) && Prev.EndVertexUp == FIntVector(1, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_NegativeVoxelSizeScalesNegatively,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalVerticesWithNegativeVoxelSize.StartVertexDownScalesNegatively",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_NegativeVoxelSizeScalesNegatively::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(2, -3, 4);
	FIntVector B(-1, 0, 5);
	FVirtualVoxelFace Face(FVoxel(), A, B, FIntVector(0), FIntVector(0));
	double VoxelSize = -2.0;
	const float Epsilon = 1e-6f;
	FVector ExpectedA = static_cast<FVector>(A) * (float)VoxelSize;
	// Act
	FVector ActualA = Face.GetFinalStartVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexDown scales negatively with negative VoxelSize"), ActualA.Equals(ExpectedA, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalEndVertexDown_NegativeVoxelSizeScalesNegatively,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalVerticesWithNegativeVoxelSize.EndVertexDownScalesNegatively",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalEndVertexDown_NegativeVoxelSizeScalesNegatively::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(2, -3, 4);
	FIntVector B(-1, 0, 5);
	FVirtualVoxelFace Face(FVoxel(), A, B, FIntVector(0), FIntVector(0));
	double VoxelSize = -2.0;
	const float Epsilon = 1e-6f;
	FVector ExpectedB = static_cast<FVector>(B) * (float)VoxelSize;
	// Act
	FVector ActualB = Face.GetFinalEndVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalEndVertexDown scales negatively with negative VoxelSize"), ActualB.Equals(ExpectedB, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_PreservesMagnitudeForLargeVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalVerticesWithLargeVoxelSize.StartVertexDownPreservesMagnitude",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_GetFinalStartVertexDown_PreservesMagnitudeForLargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	FIntVector A(100, 200, -300);
	FVirtualVoxelFace Face(FVoxel(), A, FIntVector(0), FIntVector(0), FIntVector(0));
	double VoxelSize = 1000000.0; // large voxel size
	const float Epsilon = 1e-2f; // allow some floating error at large magnitude
	FVector Expected = static_cast<FVector>(A) * (float)VoxelSize;
	// Act
	FVector Actual = Face.GetFinalStartVertexDown(VoxelSize);
	// Assert
	TestTrue(TEXT("GetFinalStartVertexDown preserves scale for large VoxelSize"), Actual.Equals(Expected, Epsilon));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength1.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	// Act
	FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateRightFace(V, Init, 1);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=1 StartVertexDown equals Initial + (0,0,0)"), Face1.StartVertexDown == Init + FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength1.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	// Act
	FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateRightFace(V, Init, 1);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=1 EndVertexDown equals Initial + (1,0,0)"), Face1.EndVertexDown == Init + FIntVector(1, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength1.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	// Act
	FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateRightFace(V, Init, 1);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=1 EndVertexUp equals Initial + (1,0,1)"), Face1.EndVertexUp == Init + FIntVector(1, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength1.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength1_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	// Act
	FVirtualVoxelFace Face1 = FVirtualVoxelFace::CreateRightFace(V, Init, 1);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=1 StartVertexUp equals Initial + (0,0,1)"), Face1.StartVertexUp == Init + FIntVector(0, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength4.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	int RunLength = 4;
	// Act
	FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateRightFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=4 StartVertexDown equals Initial + (0,RunLength-1,0)"), Face2.StartVertexDown == Init + FIntVector(0, RunLength - 1, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength4.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	int RunLength = 4;
	// Act
	FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateRightFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=4 EndVertexDown equals Initial + (1,RunLength-1,0)"), Face2.EndVertexDown == Init + FIntVector(1, RunLength - 1, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength4.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	int RunLength = 4;
	// Act
	FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateRightFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=4 EndVertexUp equals Initial + (1,RunLength-1,1)"), Face2.EndVertexUp == Init + FIntVector(1, RunLength - 1, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLength4.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateRightFace_RunLength4_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V;
	FIntVector Init(1, 2, 3);
	int RunLength = 4;
	// Act
	FVirtualVoxelFace Face2 = FVirtualVoxelFace::CreateRightFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateRightFace with RunLength=4 StartVertexUp equals Initial + (0,RunLength-1,1)"), Face2.StartVertexUp == Init + FIntVector(0, RunLength - 1, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateTopFace_PreservesVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.PreservesVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateTopFace_PreservesVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(42);
	FIntVector Init(2, 4, 6);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateTopFace preserves voxel"), Face.Voxel == V);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateTopFace_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateTopFace_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(42);
	FIntVector Init(2, 4, 6);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateTopFace StartVertexDown equals Initial + (0,0,0)"), Face.StartVertexDown == Init + FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateTopFace_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateTopFace_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(42);
	FIntVector Init(2, 4, 6);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateTopFace EndVertexDown equals Initial + (0,RunLength,0)"), Face.EndVertexDown == Init + FIntVector(0, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateTopFace_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateTopFace_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(42);
	FIntVector Init(2, 4, 6);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateTopFace EndVertexUp equals Initial + (1,RunLength,0)"), Face.EndVertexUp == Init + FIntVector(1, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateTopFace_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateTopFace_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(42);
	FIntVector Init(2, 4, 6);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateTopFace StartVertexUp equals Initial + (1,0,0)"), Face.StartVertexUp == Init + FIntVector(1, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBottomFace_PreservesVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.PreservesVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBottomFace_PreservesVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(7);
	FIntVector Init(3, 5, 1);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBottomFace preserves voxel"), Face.Voxel == V);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBottomFace_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBottomFace_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(7);
	FIntVector Init(3, 5, 1);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBottomFace StartVertexDown equals Initial + (0,RunLength,0)"), Face.StartVertexDown == Init + FIntVector(0, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBottomFace_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBottomFace_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(7);
	FIntVector Init(3, 5, 1);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBottomFace EndVertexDown equals Initial + (0,0,0)"), Face.EndVertexDown == Init + FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBottomFace_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBottomFace_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(7);
	FIntVector Init(3, 5, 1);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBottomFace EndVertexUp equals Initial + (1,0,0)"), Face.EndVertexUp == Init + FIntVector(1, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBottomFace_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBottomFace_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(7);
	FIntVector Init(3, 5, 1);
	int RunLength = 3;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBottomFace StartVertexUp equals Initial + (1,RunLength,0)"), Face.StartVertexUp == Init + FIntVector(1, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateLeftFace_PreservesVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.PreservesVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateLeftFace_PreservesVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(99);
	FIntVector Init(0, 0, 0);
	int RunLength = 5;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateLeftFace preserves voxel"), Face.Voxel == V);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateLeftFace_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateLeftFace_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(99);
	FIntVector Init(0, 0, 0);
	int RunLength = 5;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateLeftFace StartVertexDown equals Initial + (1,0,0)"), Face.StartVertexDown == Init + FIntVector(1, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateLeftFace_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateLeftFace_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(99);
	FIntVector Init(0, 0, 0);
	int RunLength = 5;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateLeftFace EndVertexDown equals Initial + (0,0,0)"), Face.EndVertexDown == Init + FIntVector(0, 0, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateLeftFace_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateLeftFace_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(99);
	FIntVector Init(0, 0, 0);
	int RunLength = 5;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateLeftFace EndVertexUp equals Initial + (0,0,1)"), Face.EndVertexUp == Init + FIntVector(0, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateLeftFace_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateLeftFace_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(99);
	FIntVector Init(0, 0, 0);
	int RunLength = 5;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateLeftFace StartVertexUp equals Initial + (1,0,1)"), Face.StartVertexUp == Init + FIntVector(1, 0, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBackFace_PreservesVoxel,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.PreservesVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBackFace_PreservesVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(123);
	FIntVector Init(9, 8, 7);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBackFace preserves voxel"), Face.Voxel == V);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBackFace_StartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.StartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBackFace_StartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(123);
	FIntVector Init(9, 8, 7);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBackFace StartVertexDown equals Initial"), Face.StartVertexDown == Init);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBackFace_EndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.EndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBackFace_EndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(123);
	FIntVector Init(9, 8, 7);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBackFace EndVertexDown equals Initial + (0,RunLength,0)"), Face.EndVertexDown == Init + FIntVector(0, RunLength, 0));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBackFace_EndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.EndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBackFace_EndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(123);
	FIntVector Init(9, 8, 7);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBackFace EndVertexUp equals Initial + (0,RunLength,1)"), Face.EndVertexUp == Init + FIntVector(0, RunLength, 1));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVirtualVoxelFace_CreateBackFace_StartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.StartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVirtualVoxelFace_CreateBackFace_StartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	FVoxel V(123);
	FIntVector Init(9, 8, 7);
	int RunLength = 2;
	// Act
	FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(V, Init, RunLength);
	// Assert
	TestTrue(TEXT("CreateBackFace StartVertexUp equals Initial + (0,0,1)"), Face.StartVertexUp == Init + FIntVector(0, 0, 1));
	return true;
}
