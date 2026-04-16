#include "VoxelMesher/MeshingUtil/VirtualVoxelFace.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength, 0)", Face.StartVertexDown, FIntVector(0, 1, 0));
	TestEqual("EndVertexDown should be InitialPosition + (0, 0, 0)", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(0, 0, 1));
	TestEqual("StartVertexUp should be InitialPosition + (0, RunLength, 1)", Face.StartVertexUp, FIntVector(0, 1, 1));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength, 0)", Face.StartVertexDown, FIntVector(1, 7, 3));
	TestEqual("EndVertexDown should be InitialPosition + (0, 0, 0)", Face.EndVertexDown, FIntVector(1, 2, 3));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(1, 2, 4));
	TestEqual("StartVertexUp should be InitialPosition + (0, RunLength, 1)", Face.StartVertexUp, FIntVector(1, 7, 4));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBackFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBackFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition + (0, RunLength, 0)", Face.EndVertexDown, FIntVector(0, 1, 0));
	TestEqual("EndVertexUp should be InitialPosition + (0, RunLength, 1)", Face.EndVertexUp, FIntVector(0, 1, 1));
	TestEqual("StartVertexUp should be InitialPosition + (0, 0, 1)", Face.StartVertexUp, FIntVector(0, 0, 1));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition", Face.StartVertexDown, FIntVector(1, 2, 3));
	TestEqual("EndVertexDown should be InitialPosition + (0, RunLength, 0)", Face.EndVertexDown, FIntVector(1, 7, 3));
	TestEqual("EndVertexUp should be InitialPosition + (0, RunLength, 1)", Face.EndVertexUp, FIntVector(1, 7, 4));
	TestEqual("StartVertexUp should be InitialPosition + (0, 0, 1)", Face.StartVertexUp, FIntVector(1, 2, 4));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (1, 0, 0)", Face.StartVertexDown, FIntVector(1, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(0, 0, 1));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 1)", Face.StartVertexUp, FIntVector(1, 0, 1));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (1, 0, 0)", Face.StartVertexDown, FIntVector(2, 2, 3));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(1, 2, 3));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(1, 2, 4));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 1)", Face.StartVertexUp, FIntVector(2, 2, 4));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateRightFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateRightFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength - 1, 0)", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition + (1, RunLength - 1, 0)", Face.EndVertexDown, FIntVector(1, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, RunLength - 1, 1)", Face.EndVertexUp, FIntVector(1, 0, 1));
	TestEqual("StartVertexUp should be InitialPosition + (0, RunLength - 1, 1)", Face.StartVertexUp, FIntVector(0, 0, 1));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateRightFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateRightFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength - 1, 0)", Face.StartVertexDown, FIntVector(1, 6, 3));
	TestEqual("EndVertexDown should be InitialPosition + (1, RunLength - 1, 0)", Face.EndVertexDown, FIntVector(2, 6, 3));
	TestEqual("EndVertexUp should be InitialPosition + (1, RunLength - 1, 1)", Face.EndVertexUp, FIntVector(2, 6, 4));
	TestEqual("StartVertexUp should be InitialPosition + (0, RunLength - 1, 1)", Face.StartVertexUp, FIntVector(1, 6, 4));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateTopFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateTopFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, 0, 0)", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition + (0, RunLength, 0)", Face.EndVertexDown, FIntVector(0, 1, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, RunLength, 0)", Face.EndVertexUp, FIntVector(1, 1, 0));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 0)", Face.StartVertexUp, FIntVector(1, 0, 0));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateTopFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateTopFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, 0, 0)", Face.StartVertexDown, FIntVector(1, 2, 3));
	TestEqual("EndVertexDown should be InitialPosition + (0, RunLength, 0)", Face.EndVertexDown, FIntVector(1, 7, 3));
	TestEqual("EndVertexUp should be InitialPosition + (1, RunLength, 0)", Face.EndVertexUp, FIntVector(2, 7, 3));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 0)", Face.StartVertexUp, FIntVector(2, 2, 3));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 1;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength, 0)", Face.StartVertexDown, FIntVector(0, 1, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, 0, 0)", Face.EndVertexUp, FIntVector(1, 0, 0));
	TestEqual("StartVertexUp should be InitialPosition + (1, RunLength, 0)", Face.StartVertexUp, FIntVector(1, 1, 0));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace_RunLengthGreaterThanOne,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.RunLengthGreaterThanOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace_RunLengthGreaterThanOne::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(2);
	const FIntVector InitialPosition(1, 2, 3);
	const int RunLength = 5;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, RunLength, 0)", Face.StartVertexDown, FIntVector(1, 7, 3));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(1, 2, 3));
	TestEqual("EndVertexUp should be InitialPosition + (1, 0, 0)", Face.EndVertexUp, FIntVector(2, 2, 3));
	TestEqual("StartVertexUp should be InitialPosition + (1, RunLength, 0)", Face.StartVertexUp, FIntVector(2, 7, 3));
	TestEqual("Voxel should match", Face.Voxel, Voxel);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionX.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 1), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionX(Face, NewFace);

	// Assert
	TestTrue("Should return true when Face.StartVertexDown.Z > NewFace.StartVertexUp.Z", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionX.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 1), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionX(Face, NewFace);

	// Assert
	TestFalse("Should return false when Face.StartVertexDown.Z <= NewFace.StartVertexUp.Z", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionRightY.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(2, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace);

	// Assert
	TestTrue("Should return true when Face.StartVertexUp.X > NewFace.EndVertexDown.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionRightY.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(2, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace);

	// Assert
	TestFalse("Should return false when Face.StartVertexUp.X <= NewFace.EndVertexDown.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionLeftY.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(2, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace);

	// Assert
	TestTrue("Should return true when Face.EndVertexUp.X > NewFace.StartVertexUp.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionLeftY.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(2, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace);

	// Assert
	TestFalse("Should return false when Face.EndVertexUp.X <= NewFace.StartVertexUp.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionZ_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionZ.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionZ_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(2, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(1, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionZ(Face, NewFace);

	// Assert
	TestTrue("Should return true when Face.StartVertexDown.X > NewFace.StartVertexUp.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionZ_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionZ.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionZ_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(1, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(2, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionZ(Face, NewFace);

	// Assert
	TestFalse("Should return false when Face.StartVertexDown.X <= NewFace.StartVertexUp.X", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);

	// Assert
	TestTrue("Should return true and merge when conditions met", Result);
	TestEqual("EndVertexDown should be updated", PrevFace.EndVertexDown, FIntVector(0, 2, 0));
	TestEqual("EndVertexUp should be updated", PrevFace.EndVertexUp, FIntVector(0, 2, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 2, 0), FIntVector(0, 3, 0), FIntVector(0, 3, 1), FIntVector(0, 2, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when EndVertexDown != StartVertexDown", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceStart(PrevFace, NewFace);

	// Assert
	TestTrue("Should return true and merge when conditions met", Result);
	TestEqual("StartVertexDown should be updated", PrevFace.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("StartVertexUp should be updated", PrevFace.StartVertexUp, FIntVector(0, 0, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 3, 0), FIntVector(0, 4, 0), FIntVector(0, 4, 1), FIntVector(0, 3, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceStart(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when StartVertexUp != EndVertexUp", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_True,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.True",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_True::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 0, 1), FIntVector(0, 1, 1), FIntVector(0, 1, 2), FIntVector(0, 0, 2));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceUp(PrevFace, NewFace);

	// Assert
	TestTrue("Should return true and merge when conditions met", Result);
	TestEqual("StartVertexUp should be updated", PrevFace.StartVertexUp, FIntVector(0, 0, 2));
	TestEqual("EndVertexUp should be updated", PrevFace.EndVertexUp, FIntVector(0, 1, 2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_False,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.False",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_False::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 0, 2), FIntVector(0, 1, 2), FIntVector(0, 1, 3), FIntVector(0, 0, 3));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceUp(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when StartVertexUp != StartVertexDown of NewFace", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetXFromVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetXFromVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetXFromVector::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(5, 10, 15);

	// Act
	const int Result = FVirtualVoxelFace::GetXFromVector(Position);

	// Assert
	TestEqual("Should return X component", Result, 5);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetYFromVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetYFromVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetYFromVector::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(5, 10, 15);

	// Act
	const int Result = FVirtualVoxelFace::GetYFromVector(Position);

	// Assert
	TestEqual("Should return Y component", Result, 10);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetZFromVector,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetZFromVector",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetZFromVector::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(5, 10, 15);

	// Act
	const int Result = FVirtualVoxelFace::GetZFromVector(Position);

	// Assert
	TestEqual("Should return Z component", Result, 15);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(1, 2, 3), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const double VoxelSize = 2.0;

	// Act
	const FVector Result = Face.GetFinalStartVertexDown(VoxelSize);

	// Assert
	TestEqual("Should return StartVertexDown multiplied by VoxelSize", Result, FVector(2.0, 4.0, 6.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 2, 3));
	const double VoxelSize = 2.0;

	// Act
	const FVector Result = Face.GetFinalStartVertexUp(VoxelSize);

	// Assert
	TestEqual("Should return StartVertexUp multiplied by VoxelSize", Result, FVector(2.0, 4.0, 6.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexDown",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(1, 2, 3), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const double VoxelSize = 2.0;

	// Act
	const FVector Result = Face.GetFinalEndVertexDown(VoxelSize);

	// Assert
	TestEqual("Should return EndVertexDown multiplied by VoxelSize", Result, FVector(2.0, 4.0, 6.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexUp,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexUp",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexUp::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(1, 2, 3), FIntVector(0, 0, 0));
	const double VoxelSize = 2.0;

	// Act
	const FVector Result = Face.GetFinalEndVertexUp(VoxelSize);

	// Assert
	TestEqual("Should return EndVertexUp multiplied by VoxelSize", Result, FVector(2.0, 4.0, 6.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_ZeroRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.ZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_ZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(0, 0, 1));
	TestEqual("StartVertexUp should be InitialPosition + (0, 0, 1)", Face.StartVertexUp, FIntVector(0, 0, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_NegativeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFrontFace.NegativeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateFrontFace_NegativeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(3);
	const FIntVector InitialPosition(-5, -10, -3);
	const int RunLength = 3;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should handle negative coordinates", Face.StartVertexDown, FIntVector(-5, -7, -3));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(-5, -10, -3));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(-5, -10, -2));
	TestEqual("StartVertexUp should handle negative coordinates", Face.StartVertexUp, FIntVector(-5, -7, -2));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_DifferentVoxelId,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.DifferentVoxelId",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_DifferentVoxelId::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(999);
	const FIntVector InitialPosition(10, 20, 30);
	const int RunLength = 2;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("Voxel ID should be preserved", Face.Voxel, Voxel);
	TestEqual("StartVertexDown should be correct", Face.StartVertexDown, FIntVector(10, 20, 30));
	TestEqual("EndVertexDown should be correct", Face.EndVertexDown, FIntVector(10, 22, 30));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace_ZeroRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateLeftFace.ZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateLeftFace_ZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateLeftFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (1, 0, 0)", Face.StartVertexDown, FIntVector(1, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (0, 0, 1)", Face.EndVertexUp, FIntVector(0, 0, 1));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 1)", Face.StartVertexUp, FIntVector(1, 0, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateRightFace_ZeroRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateRightFace.ZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateRightFace_ZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition + (0, -1, 0)", Face.StartVertexDown, FIntVector(0, -1, 0));
	TestEqual("EndVertexDown should be InitialPosition + (1, -1, 0)", Face.EndVertexDown, FIntVector(1, -1, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, -1, 1)", Face.EndVertexUp, FIntVector(1, -1, 1));
	TestEqual("StartVertexUp should be InitialPosition + (0, -1, 1)", Face.StartVertexUp, FIntVector(0, -1, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateTopFace_ZeroRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateTopFace.ZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateTopFace_ZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, 0, 0)", Face.EndVertexUp, FIntVector(1, 0, 0));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 0)", Face.StartVertexUp, FIntVector(1, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace_ZeroRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBottomFace.ZeroRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBottomFace_ZeroRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 0;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBottomFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown should be InitialPosition", Face.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexDown should be InitialPosition", Face.EndVertexDown, FIntVector(0, 0, 0));
	TestEqual("EndVertexUp should be InitialPosition + (1, 0, 0)", Face.EndVertexUp, FIntVector(1, 0, 0));
	TestEqual("StartVertexUp should be InitialPosition + (1, 0, 0)", Face.StartVertexUp, FIntVector(1, 0, 0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_DifferentVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.DifferentVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_DifferentVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(2);
	FVirtualVoxelFace PrevFace(Voxel1, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel2, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when voxels are different", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_DifferentVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.DifferentVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_DifferentVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(3);
	FVirtualVoxelFace PrevFace(Voxel1, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));
	const FVirtualVoxelFace NewFace(Voxel2, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceStart(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when voxels are different", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_DifferentVoxels,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.DifferentVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_DifferentVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel1(1);
	const FVoxel Voxel2(4);
	FVirtualVoxelFace PrevFace(Voxel1, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel2, FIntVector(0, 0, 1), FIntVector(0, 1, 1), FIntVector(0, 1, 2), FIntVector(0, 0, 2));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceUp(PrevFace, NewFace);

	// Assert
	TestFalse("Should return false when voxels are different", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexDown_ZeroVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexDown.ZeroVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexDown_ZeroVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(5, 10, 15), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const double VoxelSize = 0.0;

	// Act
	const FVector Result = Face.GetFinalStartVertexDown(VoxelSize);

	// Assert
	TestEqual("Should return zero vector when VoxelSize is 0", Result, FVector(0.0, 0.0, 0.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexUp_LargeVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalStartVertexUp.LargeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalStartVertexUp_LargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(2, 3, 4));
	const double VoxelSize = 100.0;

	// Act
	const FVector Result = Face.GetFinalStartVertexUp(VoxelSize);

	// Assert
	TestEqual("Should handle large VoxelSize", Result, FVector(200.0, 300.0, 400.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown_FractionalVoxelSize,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexDown.FractionalVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown_FractionalVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(4, 6, 8), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const double VoxelSize = 0.5;

	// Act
	const FVector Result = Face.GetFinalEndVertexDown(VoxelSize);

	// Assert
	TestEqual("Should handle fractional VoxelSize", Result, FVector(2.0, 3.0, 4.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexUp_NegativeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexUp.NegativeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexUp_NegativeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(-3, -5, -7), FIntVector(0, 0, 0));
	const double VoxelSize = 2.0;

	// Act
	const FVector Result = Face.GetFinalEndVertexUp(VoxelSize);

	// Assert
	TestEqual("Should handle negative coordinates", Result, FVector(-6.0, -10.0, -14.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetXFromVector_NegativeValue,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetXFromVector.NegativeValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetXFromVector_NegativeValue::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(-100, 50, 75);

	// Act
	const int Result = FVirtualVoxelFace::GetXFromVector(Position);

	// Assert
	TestEqual("Should return negative X component", Result, -100);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetYFromVector_ZeroValue,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetYFromVector.ZeroValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetYFromVector_ZeroValue::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(10, 0, 20);

	// Act
	const int Result = FVirtualVoxelFace::GetYFromVector(Position);

	// Assert
	TestEqual("Should return zero Y component", Result, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetZFromVector_LargeValue,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetZFromVector.LargeValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetZFromVector_LargeValue::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector Position(5, 10, 10000);

	// Act
	const int Result = FVirtualVoxelFace::GetZFromVector(Position);

	// Assert
	TestEqual("Should return large Z component", Result, 10000);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_EqualValues,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionX.EqualValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionX_EqualValues::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 5), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 5));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionX(Face, NewFace);

	// Assert
	TestFalse("Should return false when Z values are equal", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_EqualValues,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionRightY.EqualValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionRightY_EqualValues::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(3, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(3, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionRightY(Face, NewFace);

	// Assert
	TestFalse("Should return false when X values are equal", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_MultipleVertexUpdate,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.MultipleVertexUpdate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_MultipleVertexUpdate::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));
	const FVirtualVoxelFace NewFace2(Voxel, FIntVector(0, 2, 0), FIntVector(0, 3, 0), FIntVector(0, 3, 1), FIntVector(0, 2, 1));

	// Act
	const bool Result1 = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);
	const bool Result2 = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace2);

	// Assert
	TestTrue("First merge should succeed", Result1);
	TestTrue("Second merge should succeed", Result2);
	TestEqual("Final EndVertexDown after sequential merge", PrevFace.EndVertexDown, FIntVector(0, 3, 0));
	TestEqual("Final EndVertexUp after sequential merge", PrevFace.EndVertexUp, FIntVector(0, 3, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_SequentialMergeStart,
	"RDM.RDMMeshersTests.VirtualVoxelFace.SequentialMergeStart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_SequentialMergeStart::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace Face1(Voxel, FIntVector(0, 2, 0), FIntVector(0, 3, 0), FIntVector(0, 3, 1), FIntVector(0, 2, 1));
	const FVirtualVoxelFace Face2(Voxel, FIntVector(0, 1, 0), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 1));
	const FVirtualVoxelFace Face3(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));

	// Act
	const bool Result1 = FVirtualVoxelFace::MergeFaceStart(Face1, Face2);
	const bool Result2 = FVirtualVoxelFace::MergeFaceStart(Face1, Face3);

	// Assert
	TestTrue("First merge should succeed", Result1);
	TestTrue("Second merge should succeed", Result2);
	TestEqual("Final StartVertexDown after sequential merge", Face1.StartVertexDown, FIntVector(0, 0, 0));
	TestEqual("Final StartVertexUp after sequential merge", Face1.StartVertexUp, FIntVector(0, 0, 1));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_NegativeToPositiveCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFace.NegativeToPositiveCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_NegativeToPositiveCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(-10, -5, -2);
	const int RunLength = 20;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("Should handle transition from negative to positive Y", Face.StartVertexDown.Y, 10);
	TestEqual("Should preserve negative X coordinate", Face.StartVertexDown.X, -10);
	TestEqual("Should preserve negative Z coordinate", Face.EndVertexUp.Z, -1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_VertexTransformationConsistency,
	"RDM.RDMMeshersTests.VirtualVoxelFace.VertexTransformationConsistency",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_VertexTransformationConsistency::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(2, 3, 4), FIntVector(5, 6, 7), FIntVector(8, 9, 10), FIntVector(11, 12, 13));
	const double VoxelSize1 = 1.0;
	const double VoxelSize2 = 2.0;

	// Act
	const FVector Result1Start = Face.GetFinalStartVertexDown(VoxelSize1);
	const FVector Result2Start = Face.GetFinalStartVertexDown(VoxelSize2);
	const FVector Result1End = Face.GetFinalEndVertexUp(VoxelSize1);
	const FVector Result2End = Face.GetFinalEndVertexUp(VoxelSize2);

	// Assert
	TestEqual("Transformation with scale 2 should be double of scale 1", Result2Start, Result1Start * 2.0);
	TestEqual("Transformation with scale 2 should be double of scale 1", Result2End, Result1End * 2.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_OnlyEndVerticesChange,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.OnlyEndVerticesChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_OnlyEndVerticesChange::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(1, 2, 3), FIntVector(4, 5, 6), FIntVector(7, 8, 9), FIntVector(10, 11, 12));
	const FIntVector OriginalStart = PrevFace.StartVertexDown;
	const FIntVector OriginalStartUp = PrevFace.StartVertexUp;
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(4, 5, 6), FIntVector(13, 14, 15), FIntVector(16, 17, 18), FIntVector(7, 8, 9));

	// Act
	FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);

	// Assert
	TestEqual("StartVertexDown should not change", PrevFace.StartVertexDown, OriginalStart);
	TestEqual("StartVertexUp should not change", PrevFace.StartVertexUp, OriginalStartUp);
	TestEqual("EndVertexDown should be updated", PrevFace.EndVertexDown, FIntVector(13, 14, 15));
	TestEqual("EndVertexUp should be updated", PrevFace.EndVertexUp, FIntVector(16, 17, 18));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_OnlyStartVerticesChange,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceStart.OnlyStartVerticesChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceStart_OnlyStartVerticesChange::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(5, 6, 7), FIntVector(8, 9, 10), FIntVector(11, 12, 13), FIntVector(14, 15, 16));
	const FIntVector OriginalEnd = PrevFace.EndVertexDown;
	const FIntVector OriginalEndUp = PrevFace.EndVertexUp;
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(1, 2, 3), FIntVector(5, 6, 7), FIntVector(14, 15, 16), FIntVector(4, 5, 6));

	// Act
	FVirtualVoxelFace::MergeFaceStart(PrevFace, NewFace);

	// Assert
	TestEqual("EndVertexDown should not change", PrevFace.EndVertexDown, OriginalEnd);
	TestEqual("EndVertexUp should not change", PrevFace.EndVertexUp, OriginalEndUp);
	TestEqual("StartVertexDown should be updated", PrevFace.StartVertexDown, FIntVector(1, 2, 3));
	TestEqual("StartVertexUp should be updated", PrevFace.StartVertexUp, FIntVector(4, 5, 6));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_OnlyUpVerticesChange,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceUp.OnlyUpVerticesChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceUp_OnlyUpVerticesChange::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(1, 2, 3), FIntVector(4, 5, 6), FIntVector(7, 8, 9), FIntVector(10, 11, 12));
	const FIntVector OriginalStartDown = PrevFace.StartVertexDown;
	const FIntVector OriginalEndDown = PrevFace.EndVertexDown;
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(1, 2, 9), FIntVector(4, 5, 9), FIntVector(13, 14, 15), FIntVector(16, 17, 18));

	// Act
	FVirtualVoxelFace::MergeFaceUp(PrevFace, NewFace);

	// Assert
	TestEqual("StartVertexDown should not change", PrevFace.StartVertexDown, OriginalStartDown);
	TestEqual("EndVertexDown should not change", PrevFace.EndVertexDown, OriginalEndDown);
	TestEqual("StartVertexUp should be updated", PrevFace.StartVertexUp, FIntVector(16, 17, 18));
	TestEqual("EndVertexUp should be updated", PrevFace.EndVertexUp, FIntVector(13, 14, 15));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateFacePreservesVoxelThroughTransform,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateFacePreservesVoxelThroughTransform",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateFacePreservesVoxelThroughTransform::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(42);
	const FIntVector InitialPosition(10, 20, 30);
	const int RunLength = 7;

	// Act
	const FVirtualVoxelFace FrontFace = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace BackFace = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace LeftFace = FVirtualVoxelFace::CreateLeftFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace RightFace = FVirtualVoxelFace::CreateRightFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace TopFace = FVirtualVoxelFace::CreateTopFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace BottomFace = FVirtualVoxelFace::CreateBottomFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("FrontFace preserves voxel ID", FrontFace.Voxel.VoxelId, 42);
	TestEqual("BackFace preserves voxel ID", BackFace.Voxel.VoxelId, 42);
	TestEqual("LeftFace preserves voxel ID", LeftFace.Voxel.VoxelId, 42);
	TestEqual("RightFace preserves voxel ID", RightFace.Voxel.VoxelId, 42);
	TestEqual("TopFace preserves voxel ID", TopFace.Voxel.VoxelId, 42);
	TestEqual("BottomFace preserves voxel ID", BottomFace.Voxel.VoxelId, 42);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_BoundaryValue,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFailConditionLeftY.BoundaryValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFailConditionLeftY_BoundaryValue::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(5, 0, 0));
	const FVirtualVoxelFace NewFace(FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(0, 0, 0), FIntVector(5, 0, 0));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFailConditionLeftY(Face, NewFace);

	// Assert
	TestFalse("Should return false when values are equal at boundary", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown_AllNegativeCoordinates,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetFinalEndVertexDown.AllNegativeCoordinates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetFinalEndVertexDown_AllNegativeCoordinates::RunTest(const FString& Parameters)
{
	// Arrange
	const FVirtualVoxelFace Face(FIntVector(0, 0, 0), FIntVector(-10, -20, -30), FIntVector(0, 0, 0), FIntVector(0, 0, 0));
	const double VoxelSize = 1.5;

	// Act
	const FVector Result = Face.GetFinalEndVertexDown(VoxelSize);

	// Assert
	TestEqual("Should correctly scale negative coordinates", Result, FVector(-15.0, -30.0, -45.0));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_GetXFromVector_MaximumValue,
	"RDM.RDMMeshersTests.VirtualVoxelFace.GetXFromVector.MaximumValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_GetXFromVector_MaximumValue::RunTest(const FString& Parameters)
{
	// Arrange
	const int MaxInt = 2147483647;
	const FIntVector Position(MaxInt, 0, 0);

	// Act
	const int Result = FVirtualVoxelFace::GetXFromVector(Position);

	// Assert
	TestEqual("Should return maximum integer value", Result, MaxInt);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_LargeNegativePosition,
	"RDM.RDMMeshersTests.VirtualVoxelFace.CreateBackFace.LargeNegativePosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_CreateBackFace_LargeNegativePosition::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(-1000, -2000, -3000);
	const int RunLength = 100;

	// Act
	const FVirtualVoxelFace Face = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("StartVertexDown handles large negative coords", Face.StartVertexDown, FIntVector(-1000, -2000, -3000));
	TestEqual("EndVertexDown with large negative and RunLength", Face.EndVertexDown, FIntVector(-1000, -1900, -3000));
	TestEqual("EndVertexUp handles large negative Z", Face.EndVertexUp, FIntVector(-1000, -1900, -2999));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_FailWithOffByOneError,
	"RDM.RDMMeshersTests.VirtualVoxelFace.MergeFaceEnd.FailWithOffByOneError",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_MergeFaceEnd_FailWithOffByOneError::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	FVirtualVoxelFace PrevFace(Voxel, FIntVector(0, 0, 0), FIntVector(0, 1, 0), FIntVector(0, 1, 1), FIntVector(0, 0, 1));
	const FVirtualVoxelFace NewFace(Voxel, FIntVector(0, 1, 1), FIntVector(0, 2, 0), FIntVector(0, 2, 1), FIntVector(0, 1, 2));

	// Act
	const bool Result = FVirtualVoxelFace::MergeFaceEnd(PrevFace, NewFace);

	// Assert
	TestFalse("Should fail when down vertices don't match due to Z mismatch", Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_VeryLargeRunLength,
	"RDM.RDMMeshersTests.VirtualVoxelFace.VeryLargeRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_VeryLargeRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	const FVoxel Voxel(1);
	const FIntVector InitialPosition(0, 0, 0);
	const int RunLength = 10000;

	// Act
	const FVirtualVoxelFace FrontFace = FVirtualVoxelFace::CreateFrontFace(Voxel, InitialPosition, RunLength);
	const FVirtualVoxelFace BackFace = FVirtualVoxelFace::CreateBackFace(Voxel, InitialPosition, RunLength);

	// Assert
	TestEqual("FrontFace StartVertexDown with large RunLength", FrontFace.StartVertexDown.Y, 10000);
	TestEqual("BackFace EndVertexDown with large RunLength", BackFace.EndVertexDown.Y, 10000);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_VirtualVoxelFace_AllComponentsZero,
	"RDM.RDMMeshersTests.VirtualVoxelFace.AllComponentsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_VirtualVoxelFace_AllComponentsZero::RunTest(const FString& Parameters)
{
	// Arrange
	const FIntVector ZeroVector(0, 0, 0);

	// Act
	const int X = FVirtualVoxelFace::GetXFromVector(ZeroVector);
	const int Y = FVirtualVoxelFace::GetYFromVector(ZeroVector);
	const int Z = FVirtualVoxelFace::GetZFromVector(ZeroVector);

	// Assert
	TestEqual("All components should be zero", X, 0);
	TestEqual("All components should be zero", Y, 0);
	TestEqual("All components should be zero", Z, 0);

	return true;
}

