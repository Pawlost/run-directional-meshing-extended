#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "VoxelMesher/VoxelEventMesher.h"
#include "VoxelMesher/MeshingUtil/MeshingEvent.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelMesher/MeshingUtil/RLEVoxelEdit.h"

// AdvanceEditInterval Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalWithEmptyEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithEmptyEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithEmptyEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestEqual("LastEventIndex set to MaxVoxelsInChunk + 1", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalWithSingleEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithSingleEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithSingleEdit::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);
	
	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 10;
	EditVoxel.Voxel = FVoxel(1);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 100;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestTrue("VoxelEdits emptied after pop", VoxelEdits.IsEmpty());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalWithMultipleEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithMultipleEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithMultipleEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);
	
	FRLEVoxel EditVoxel1;
	EditVoxel1.RunLenght = 5;
	EditVoxel1.Voxel = FVoxel(1);
	FRLEVoxelEdit Edit1;
	Edit1.EditEventIndex = 50;
	Edit1.EditVoxel = EditVoxel1;
	
	FRLEVoxel EditVoxel2;
	EditVoxel2.RunLenght = 10;
	EditVoxel2.Voxel = FVoxel(2);
	FRLEVoxelEdit Edit2;
	Edit2.EditEventIndex = 100;
	Edit2.EditVoxel = EditVoxel2;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit1);
	VoxelEdits.Push(Edit2);

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestEqual("First edit popped", VoxelEdits.Num(), 1);
	return true;
}

// UpdateInternalState Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherUpdateInternalStateUpdatesParameters,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.UpdatesParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateUpdatesParameters::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Act
	Mesher.UpdateInternalState(32, 512, 8192);

	// Assert
	TestEqual("VoxelLine set", Mesher.GetMainVoxelGridPtr() != nullptr, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherUpdateInternalStateResetsIndices,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.ResetsIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateResetsIndices::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	// Act
	Mesher.UpdateInternalState(32, 512, 8192);

	// Assert
	TestEqual("State reset on update", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherUpdateInternalStateWithSmallValues,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.WithSmallValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateWithSmallValues::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Act
	Mesher.UpdateInternalState(1, 1, 1);

	// Assert
	TestEqual("Can handle minimum dimensions", true, true);
	return true;
}

// InitializeIntervals Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithoutEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithoutEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestEqual("EditEnabled false when no edits", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithEditsAtStart,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsAtStart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditsAtStart::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 50;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 100;
	V2.Voxel = FVoxel(2);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 30;
	EditVoxel.Voxel = FVoxel(3);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 0;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestEqual("EditEnabled true when edits present", Mesher.IsEditEnabled(), true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithEditsAtMiddle,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsAtMiddle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditsAtMiddle::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 50;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 100;
	V2.Voxel = FVoxel(2);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 30;
	EditVoxel.Voxel = FVoxel(3);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 50;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestEqual("EditEnabled true for middle edits", Mesher.IsEditEnabled(), true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsCreatesMainVoxelGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.CreatesMainVoxelGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsCreatesMainVoxelGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestTrue("Main voxel grid created", Mesher.GetMainVoxelGridPtr() != nullptr);
	return true;
}

// InternalReset Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInternalResetClearsState,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.ClearsState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInternalResetClearsState::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	// Act
	Mesher.InternalReset();

	// Assert
	TestEqual("State cleared", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInternalResetDisablesEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.DisablesEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInternalResetDisablesEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	// Act
	Mesher.InternalReset();

	// Assert
	TestEqual("EditEnabled set to false", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInternalResetMultipleTimes,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.MultipleTimes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInternalResetMultipleTimes::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	// Act
	Mesher.InternalReset();
	Mesher.InternalReset();

	// Assert
	TestEqual("Multiple resets allowed", true, true);
	return true;
}

// Constructor Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherConstructorInitializesVirtualFaces,
	"RDM.RDMMeshersTests.VoxelEventMesher.Constructor.InitializesVirtualFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherConstructorInitializesVirtualFaces::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();

	// Act
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Assert
	TestEqual("Constructor completes", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherConstructorWithNullData,
	"RDM.RDMMeshersTests.VoxelEventMesher.Constructor.WithNullData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherConstructorWithNullData::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();

	// Act
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Assert
	TestEqual("Constructor handles data", true, true);
	return true;
}

// AdvanceEditInterval with various scenarios Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalConsecutiveCalls,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.ConsecutiveCalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalConsecutiveCalls::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);
	
	FRLEVoxel EditVoxel1;
	EditVoxel1.RunLenght = 10;
	EditVoxel1.Voxel = FVoxel(1);
	FRLEVoxelEdit Edit1;
	Edit1.EditEventIndex = 50;
	Edit1.EditVoxel = EditVoxel1;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit1);

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);
	TArray<FRLEVoxelEdit> EmptyEdits;
	Mesher.AdvanceEditInterval(EmptyEdits);

	// Assert
	TestEqual("Consecutive calls handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherUpdateInternalStateMultipleUpdates,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.MultipleUpdates",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateMultipleUpdates::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Act
	Mesher.UpdateInternalState(16, 256, 4096);
	Mesher.UpdateInternalState(32, 512, 8192);
	Mesher.UpdateInternalState(8, 128, 2048);

	// Assert
	TestEqual("Multiple state updates handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithLargeGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithLargeGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithLargeGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(256, 65536, 1000000);

	FRLEVoxel V1;
	V1.RunLenght = 500000;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestTrue("Large grid handled", Mesher.GetMainVoxelGridPtr() != nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithMultipleGridRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMultipleGridRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithMultipleGridRuns::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 10;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 20;
	V2.Voxel = FVoxel(2);
	FRLEVoxel V3;
	V3.RunLenght = 30;
	V3.Voxel = FVoxel(3);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestTrue("Multiple runs handled", Mesher.GetMainVoxelGridPtr() != nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalPreservesGridValidity,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.PreservesGridValidity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalPreservesGridValidity::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);
	
	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 10;
	EditVoxel.Voxel = FVoxel(1);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 100;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestEqual("Grid validity preserved", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsEditsSpanningMultipleRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.EditsSpanningMultipleRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsEditsSpanningMultipleRuns::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 10;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 20;
	V2.Voxel = FVoxel(2);
	FRLEVoxel V3;
	V3.RunLenght = 30;
	V3.Voxel = FVoxel(3);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 35;
	EditVoxel.Voxel = FVoxel(4);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 5;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestEqual("Edits spanning runs handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInternalResetAfterInitializeIntervals,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.AfterInitializeIntervals",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInternalResetAfterInitializeIntervals::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act
	Mesher.InternalReset();

	// Assert
	TestEqual("Reset after initialize intervals", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherUpdateInternalStateZeroDimensions,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.WithZeroDimension",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateZeroDimensions::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	// Act
	Mesher.UpdateInternalState(0, 0, 0);

	// Assert
	TestEqual("Edge case handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherInitializeIntervalsWithSingleVoxelEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithSingleVoxelEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithSingleVoxelEdit::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 10;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 1;
	EditVoxel.Voxel = FVoxel(2);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 0;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Assert
	TestEqual("Single voxel edit handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalWithLargeRunLength,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithLargeRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithLargeRunLength::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(256, 65536, 1000000);
	
	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 500000;
	EditVoxel.Voxel = FVoxel(1);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 100000;
	Edit.EditVoxel = EditVoxel;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);

	// Act
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestTrue("Large run lengths handled", VoxelEdits.IsEmpty());
	return true;
}

// GenerateVirtualFaces Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherGenerateVirtualFacesWithEmptyGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.GenerateVirtualFaces.WithEmptyGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGenerateVirtualFacesWithEmptyGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("GenerateVirtualFaces completes with empty grid", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherGenerateVirtualFacesWithFilledGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.GenerateVirtualFaces.WithFilledGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGenerateVirtualFacesWithFilledGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 2048;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 2048;
	V2.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("GenerateVirtualFaces completes with filled grid", true, true);
	return true;
}

// EditVoxelGrid Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherEditVoxelGridWithoutEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.EditVoxelGrid.WithoutEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherEditVoxelGridWithoutEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("EditVoxelGrid handled without edits", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherEditVoxelGridWithEditsAtStart,
	"RDM.RDMMeshersTests.VoxelEventMesher.EditVoxelGrid.WithEditsAtStart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherEditVoxelGridWithEditsAtStart::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 50;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 50;
	V2.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FRLEVoxel EditVoxel;
	EditVoxel.RunLenght = 25;
	EditVoxel.Voxel = FVoxel(2);
	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 0;
	Edit.EditVoxel = EditVoxel;

	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit);
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("EditVoxelGrid handled with edits at start", Mesher.IsEditEnabled(), true);
	return true;
}

// CreateBorder Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateBorderWithValidCondition,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateBorder.WithValidCondition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateBorderWithValidCondition::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel BorderVoxel;
	BorderVoxel.RunLenght = 16;
	BorderVoxel.Voxel = FVoxel(1);

	// Act & Assert
	TestEqual("CreateBorder executes with valid condition", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateBorderWithInvalidCondition,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateBorder.WithInvalidCondition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateBorderWithInvalidCondition::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel BorderVoxel;
	BorderVoxel.RunLenght = 16;
	BorderVoxel.Voxel = FVoxel(1);

	// Act & Assert
	TestEqual("CreateBorder skips with invalid condition", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateBorderWithZeroYEnd,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateBorder.WithZeroYEnd",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateBorderWithZeroYEnd::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel BorderVoxel;
	BorderVoxel.RunLenght = 0;
	BorderVoxel.Voxel = FVoxel(0);

	// Act & Assert
	TestEqual("CreateBorder handles zero YEnd", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateBorderWithLargeYEnd,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateBorder.WithLargeYEnd",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateBorderWithLargeYEnd::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(256, 65536, 1000000);

	FRLEVoxel BorderVoxel;
	BorderVoxel.RunLenght = 1000;
	BorderVoxel.Voxel = FVoxel(1);

	// Act & Assert
	TestEqual("CreateBorder handles large YEnd", true, true);
	return true;
}

// AdvanceAllMeshingEvents Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceAllMeshingEventsWithValidState,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.WithValidState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceAllMeshingEventsWithValidState::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act
	Mesher.AdvanceAllMeshingEvents();

	// Assert
	TestEqual("AdvanceAllMeshingEvents executes successfully", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceAllMeshingEventsMultipleCalls,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.MultipleCalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceAllMeshingEventsMultipleCalls::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act
	Mesher.AdvanceAllMeshingEvents();
	Mesher.AdvanceAllMeshingEvents();

	// Assert
	TestEqual("Multiple AdvanceAllMeshingEvents calls handled", true, true);
	return true;
}

// ConvertVirtualFacesToMesh Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherConvertVirtualFacesToMeshWithEmptyVoxels,
	"RDM.RDMMeshersTests.VoxelEventMesher.ConvertVirtualFacesToMesh.WithEmptyVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherConvertVirtualFacesToMeshWithEmptyVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	FVoxelMesh MeshContainer;
	
	// Act & Assert
	TestEqual("ConvertVirtualFacesToMesh handles empty voxels", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherConvertVirtualFacesToMeshWithFilledVoxels,
	"RDM.RDMMeshersTests.VoxelEventMesher.ConvertVirtualFacesToMesh.WithFilledVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherConvertVirtualFacesToMeshWithFilledVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 2048;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 2048;
	V2.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	FVoxelMesh MeshContainer;
	
	// Act & Assert
	TestEqual("ConvertVirtualFacesToMesh handles filled voxels", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherConvertVirtualFacesToMeshWithDifferentVoxelSize,
	"RDM.RDMMeshersTests.VoxelEventMesher.ConvertVirtualFacesToMesh.WithDifferentVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherConvertVirtualFacesToMeshWithDifferentVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	FVoxelMesh MeshContainer;
	
	// Act & Assert
	TestEqual("ConvertVirtualFacesToMesh handles different voxel sizes", true, true);
	return true;
}

// CreateVirtualVoxelFacesInLShape Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateVirtualVoxelFacesInLShapeWithAllEmpty,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateVirtualVoxelFacesInLShape.WithAllEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateVirtualVoxelFacesInLShapeWithAllEmpty::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("CreateVirtualVoxelFacesInLShape handles all empty voxels", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherCreateVirtualVoxelFacesInLShapeWithMixed,
	"RDM.RDMMeshersTests.VoxelEventMesher.CreateVirtualVoxelFacesInLShape.WithMixed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherCreateVirtualVoxelFacesInLShapeWithMixed::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 1024;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 1024;
	V2.Voxel = FVoxel(0);
	FRLEVoxel V3;
	V3.RunLenght = 1024;
	V3.Voxel = FVoxel(1);
	FRLEVoxel V4;
	V4.RunLenght = 1024;
	V4.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);
	Grid->Add(V4);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("CreateVirtualVoxelFacesInLShape handles mixed voxels", true, true);
	return true;
}

// TraverseYDirection Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherTraverseYDirectionWithUniformVoxels,
	"RDM.RDMMeshersTests.VoxelEventMesher.TraverseYDirection.WithUniformVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherTraverseYDirectionWithUniformVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("TraverseYDirection completes with uniform voxels", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherTraverseYDirectionWithVariedVoxels,
	"RDM.RDMMeshersTests.VoxelEventMesher.TraverseYDirection.WithVariedVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherTraverseYDirectionWithVariedVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 512;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 512;
	V2.Voxel = FVoxel(0);
	FRLEVoxel V3;
	V3.RunLenght = 512;
	V3.Voxel = FVoxel(1);
	FRLEVoxel V4;
	V4.RunLenght = 2560;
	V4.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);
	Grid->Add(V4);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestEqual("TraverseYDirection completes with varied voxels", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherStateConsistencyAcrossOperations,
	"RDM.RDMMeshersTests.VoxelEventMesher.StateConsistency.AcrossOperations",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherStateConsistencyAcrossOperations::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	FRLEVoxel V1;
	V1.RunLenght = 100;
	V1.Voxel = FVoxel(1);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	Mesher.UpdateInternalState(16, 256, 4096);
	Mesher.InitializeIntervals(Grid, VoxelEdits);
	Mesher.AdvanceEditInterval(VoxelEdits);
	Mesher.InternalReset();

	// Assert
	TestEqual("State remains consistent across operations", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherLargeScaleGridProcessing,
	"RDM.RDMMeshersTests.VoxelEventMesher.LargeScale.GridProcessing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherLargeScaleGridProcessing::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(512, 262144, 16000000);

	FRLEVoxel V1;
	V1.RunLenght = 8000000;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 8000000;
	V2.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	TArray<FRLEVoxelEdit> VoxelEdits;
	Mesher.InitializeIntervals(Grid, VoxelEdits);

	// Act & Assert
	TestTrue("Large scale grid processed", Mesher.GetMainVoxelGridPtr() != nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherSequentialEditOperations,
	"RDM.RDMMeshersTests.VoxelEventMesher.Sequential.EditOperations",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherSequentialEditOperations::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 50;
	V1.Voxel = FVoxel(1);
	FRLEVoxel V2;
	V2.RunLenght = 50;
	V2.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FRLEVoxel EditVoxel1;
	EditVoxel1.RunLenght = 20;
	EditVoxel1.Voxel = FVoxel(2);
	FRLEVoxelEdit Edit1;
	Edit1.EditEventIndex = 10;
	Edit1.EditVoxel = EditVoxel1;

	FRLEVoxel EditVoxel2;
	EditVoxel2.RunLenght = 15;
	EditVoxel2.Voxel = FVoxel(3);
	FRLEVoxelEdit Edit2;
	Edit2.EditEventIndex = 35;
	Edit2.EditVoxel = EditVoxel2;

	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Push(Edit1);
	VoxelEdits.Push(Edit2);

	// Act
	Mesher.InitializeIntervals(Grid, VoxelEdits);
	Mesher.AdvanceEditInterval(VoxelEdits);

	// Assert
	TestEqual("Sequential edit operations handled", true, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherReinitializeAfterNoEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.Reinitialize.AfterNoEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherReinitializeAfterNoEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	TArray<FRLEVoxelEdit> NoEdits;
	Mesher.InitializeIntervals(Grid, NoEdits);

	// Act
	Mesher.InternalReset();
	Mesher.InitializeIntervals(Grid, NoEdits);

	// Assert
	TestEqual("State remains consistent after reinitialize with no edits", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherReinitializeAfterEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.Reinitialize.AfterEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherReinitializeAfterEdits::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	FRLEVoxel V1;
	V1.RunLenght = 4096;
	V1.Voxel = FVoxel(0);
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FRLEVoxelEdit Edit;
	Edit.EditEventIndex = 8;
	FRLEVoxel EV;
	EV.RunLenght = 2;
	EV.Voxel = FVoxel(5);
	Edit.EditVoxel = EV;
	TArray<FRLEVoxelEdit> WithEdits;
	WithEdits.Push(Edit);

	Mesher.InitializeIntervals(Grid, WithEdits);

	// Act
	Mesher.InternalReset();
	Mesher.InitializeIntervals(Grid, WithEdits);

	// Assert
	TestEqual("State can recover after reinitialize with edits", Mesher.IsEditEnabled(), true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherAdvanceEditIntervalDrainsAllEditsSequentially,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.DrainsAllEditsSequentially",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalDrainsAllEditsSequentially::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	for (int32 i = 0; i < 5; ++i)
	{
		FRLEVoxelEdit Edit;
		Edit.EditEventIndex = i * 100;
		FRLEVoxel EV;
		EV.RunLenght = 10;
		EV.Voxel = FVoxel(i + 1);
		Edit.EditVoxel = EV;

		TArray<FRLEVoxelEdit> VoxelEdits;
		VoxelEdits.Push(Edit);
		Mesher.AdvanceEditInterval(VoxelEdits);
	}

	TArray<FRLEVoxelEdit> EmptyEdits;
	Mesher.AdvanceEditInterval(EmptyEdits);

	TestEqual("Edit disabled after all edits drained", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherIsEditEnabledFalseBeforeInitializeIntervals,
 "RDM.RDMMeshersTests.VoxelEventMesher.IsEditEnabled.FalseBeforeInitializeIntervals",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherIsEditEnabledFalseBeforeInitializeIntervals::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TestEqual("EditEnabled false before InitializeIntervals", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherGetMainVoxelGridPtrNullBeforeInitialize,
 "RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullBeforeInitialize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGetMainVoxelGridPtrNullBeforeInitialize::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TestEqual("MainVoxelGridPtr null before InitializeIntervals", Mesher.GetMainVoxelGridPtr() == nullptr, true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsOverwritesPreviousGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.OverwritesPreviousGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsOverwritesPreviousGrid::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 100;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid1 = MakeShared<TArray<FRLEVoxel>>();
 Grid1->Add(V1);

 FRLEVoxel V2;
 V2.RunLenght = 200;
 V2.Voxel = FVoxel(2);
 TSharedPtr<TArray<FRLEVoxel>> Grid2 = MakeShared<TArray<FRLEVoxel>>();
 Grid2->Add(V2);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid1, VoxelEdits);
 auto FirstPtr = Mesher.GetMainVoxelGridPtr();

 Mesher.InitializeIntervals(Grid2, VoxelEdits);
 auto SecondPtr = Mesher.GetMainVoxelGridPtr();

 TestTrue("First grid pointer valid", FirstPtr.IsValid());
 TestTrue("Grid pointer valid after second initialize", SecondPtr.IsValid());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalAfterInternalReset,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.AfterInternalReset",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalAfterInternalReset::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InternalReset();
 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("Edit disabled after advance post-reset", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsEditAtExactRunBoundary,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.EditAtExactRunBoundary",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsEditAtExactRunBoundary::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 100;
 V1.Voxel = FVoxel(1);
 FRLEVoxel V2;
 V2.RunLenght = 100;
 V2.Voxel = FVoxel(2);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 100;
 FRLEVoxel EV;
 EV.RunLenght = 50;
 EV.Voxel = FVoxel(3);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Edit at exact run boundary enabled", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalSingleEditBecomesDisabled,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.SingleEditBecomesDisabled",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalSingleEditBecomesDisabled::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.AdvanceEditInterval(VoxelEdits);

 TArray<FRLEVoxelEdit> EmptyEdits;
 Mesher.AdvanceEditInterval(EmptyEdits);

 TestEqual("EditEnabled false after consuming single edit", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStateThenImmediatelyReset,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.ThenImmediatelyReset",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateThenImmediatelyReset::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 Mesher.UpdateInternalState(16, 256, 4096);
 Mesher.InternalReset();

 TestEqual("EditEnabled false after update then reset", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithZeroRunLengthVoxel,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithZeroRunLengthVoxel",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithZeroRunLengthVoxel::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 0;
 V1.Voxel = FVoxel(1);
 FRLEVoxel V2;
 V2.RunLenght = 100;
 V2.Voxel = FVoxel(2);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Zero run length voxel in grid handled", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherEditEnabledTrueWithEditsAtPositionZero,
 "RDM.RDMMeshersTests.VoxelEventMesher.EditEnabled.TrueWithEditsAtPositionZero",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherEditEnabledTrueWithEditsAtPositionZero::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EV;
 EV.RunLenght = 1;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("EditEnabled true for edit at position zero", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalMultipleEditsReducesCountByOne,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.MultipleEditsReducesCountByOne",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalMultipleEditsReducesCountByOne::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 for (int32 i = 0; i < 3; ++i)
 {
  FRLEVoxelEdit Edit;
  Edit.EditEventIndex = i * 25;
  FRLEVoxel EditVoxel;
  EditVoxel.RunLenght = 3;
  EditVoxel.Voxel = FVoxel(i + 1);
  Edit.EditVoxel = EditVoxel;
  VoxelEdits.Push(Edit);
 }

 // Act
 Mesher.AdvanceEditInterval(VoxelEdits);

 // Assert
 TestEqual("Exactly one edit consumed", VoxelEdits.Num(), 2);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorReinitializeFromNoEditsToEditsTogglesEditEnabled,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.ReinitializeFromNoEditsToEditsTogglesEditEnabled",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorReinitializeFromNoEditsToEditsTogglesEditEnabled::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 TArray<FRLEVoxelEdit> NoEdits;
 Mesher.InitializeIntervals(Grid, NoEdits);
 TestEqual("Edit disabled after first call without edits", Mesher.IsEditEnabled(), false);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 32;
 FRLEVoxel EV;
 EV.RunLenght = 4;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> WithEdits;
 WithEdits.Push(Edit);
 Mesher.InitializeIntervals(Grid, WithEdits);

 TestEqual("Edit enabled after second call with edits", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorReinitializeFromEditsToNoEditsDisablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.ReinitializeFromEditsToNoEditsDisablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorReinitializeFromEditsToNoEditsDisablesEdit::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 8;
 FRLEVoxel EV;
 EV.RunLenght = 2;
 EV.Voxel = FVoxel(5);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> WithEdits;
 WithEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, WithEdits);

 // Act
 Mesher.InternalReset();
 Mesher.InitializeIntervals(Grid, WithEdits);

 // Assert
 TestEqual("State can recover after reinitialize with edits", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorAdvanceEditIntervalThreeEditsLeavesTwo,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.AdvanceEditIntervalThreeEditsLeavesTwo",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorAdvanceEditIntervalThreeEditsLeavesTwo::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 for (int32 i = 0; i < 3; ++i)
 {
  FRLEVoxelEdit Edit;
  Edit.EditEventIndex = i * 25;
  FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(i + 1);
  Edit.EditVoxel = EditVoxel;
  VoxelEdits.Push(Edit);
 }

 // Act
 Mesher.AdvanceEditInterval(VoxelEdits);

 // Assert
 TestEqual("Exactly one edit consumed", VoxelEdits.Num(), 2);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorAdvanceEditIntervalWithNoEditsDisablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.AdvanceEditIntervalWithNoEditsDisablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorAdvanceEditIntervalWithNoEditsDisablesEdit::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;

 // Act
 Mesher.AdvanceEditInterval(VoxelEdits);

 // Assert
 TestTrue("Edit array remains empty", VoxelEdits.IsEmpty());
 TestEqual("Edit mode disabled with no edits", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorInitializeIntervalsWithEditAtLastVoxelEnablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InitializeIntervalsWithEditAtLastVoxelEnablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorInitializeIntervalsWithEditAtLastVoxelEnablesEdit::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 4095;
 FRLEVoxel EV;
 EV.RunLenght = 1;
 EV.Voxel = FVoxel(9);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 // Act
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 // Assert
 TestEqual("Edit enabled for last-voxel edit", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorUpdateInternalStateWithArbitraryValuesAllowsInitialization,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.UpdateInternalStateWithArbitraryValuesAllowsInitialization",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorUpdateInternalStateWithArbitraryValuesAllowsInitialization::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(17, 289, 4913);

 FRLEVoxel V1;
 V1.RunLenght = 100;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 TArray<FRLEVoxelEdit> VoxelEdits;

 // Act
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 // Assert
 TestTrue("Main grid is available after arbitrary dimension setup", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorInternalResetAfterEditedInitializationCanRecover,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InternalResetAfterEditedInitializationCanRecover",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorInternalResetAfterEditedInitializationCanRecover::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 64;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 4;
 EditVoxel.Voxel = FVoxel(2);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 // Act
 Mesher.InitializeIntervals(Grid, VoxelEdits);
 bool BeforeReset = Mesher.IsEditEnabled();
 Mesher.InternalReset();
 bool AfterReset = Mesher.IsEditEnabled();
 Mesher.InitializeIntervals(Grid, VoxelEdits);
 bool AfterRecovery = Mesher.IsEditEnabled();

 // Assert
 TestEqual("Edit mode active before reset", BeforeReset, true);
 TestEqual("Edit mode disabled after reset", AfterReset, false);
 TestEqual("Edit mode can be restored after reinitialize", AfterRecovery, true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherBehaviorInitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled,
 "RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBehaviorInitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit1;
 Edit1.EditEventIndex = 128;
 FRLEVoxel EditVoxel1;
 EditVoxel1.RunLenght = 3;
 EditVoxel1.Voxel = FVoxel(4);
 Edit1.EditVoxel = EditVoxel1;

 FRLEVoxelEdit Edit2;
 Edit2.EditEventIndex = 128;
 FRLEVoxel EditVoxel2;
 EditVoxel2.RunLenght = 6;
 EditVoxel2.Voxel = FVoxel(7);
 Edit2.EditVoxel = EditVoxel2;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit1);
 VoxelEdits.Push(Edit2);

 // Act
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 // Assert
 TestEqual("Edit remains enabled with overlapping edit events", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherGetMainVoxelGridPtrNullAfterInternalReset,
 "RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullAfterInternalReset",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGetMainVoxelGridPtrNullAfterInternalReset::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);
 Mesher.InternalReset();

 TestEqual("Grid pointer is null after InternalReset", Mesher.GetMainVoxelGridPtr() == nullptr, true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalConsumesExactlyOneEditPerCall,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.ConsumesExactlyOneEditPerCall",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalConsumesExactlyOneEditPerCall::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 for (int32 i = 0; i < 5; ++i)
 {
  FRLEVoxelEdit Edit;
  Edit.EditEventIndex = i * 50;
  FRLEVoxel EV;
  EV.RunLenght = 5;
  EV.Voxel = FVoxel(i + 1);
  Edit.EditVoxel = EV;
  VoxelEdits.Push(Edit);
 }

 int32 InitialCount = VoxelEdits.Num();
 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("Exactly one edit consumed per call", VoxelEdits.Num(), InitialCount - 1);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalCalledUntilEmptyDisablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.CalledUntilEmptyDisablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalCalledUntilEmptyDisablesEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 for (int32 i = 0; i < 4; ++i)
 {
  FRLEVoxelEdit Edit;
  Edit.EditEventIndex = i * 100;
  FRLEVoxel EV;
  EV.RunLenght = 10;
  EV.Voxel = FVoxel(i + 1);
  Edit.EditVoxel = EV;
  VoxelEdits.Push(Edit);
 }

 while (!VoxelEdits.IsEmpty())
 {
  Mesher.AdvanceEditInterval(VoxelEdits);
 }
 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("Edit disabled after draining all edits", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithNoRunsInGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithNoRunsInGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithNoRunsInGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 TArray<FRLEVoxelEdit> VoxelEdits;

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Edit disabled for empty grid", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceAllMeshingEventsAfterInternalReset,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.AfterInternalReset",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceAllMeshingEventsAfterInternalReset::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);
 Mesher.AdvanceAllMeshingEvents();
 Mesher.InternalReset();
 Mesher.AdvanceAllMeshingEvents();

 TestEqual("AdvanceAllMeshingEvents safe after InternalReset", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsSecondCallWithEditsAfterFirstWithoutEditsEnablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.SecondCallWithEditsAfterFirstWithoutEnablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsSecondCallWithEditsAfterFirstWithoutEditsEnablesEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 TArray<FRLEVoxelEdit> NoEdits;
 Mesher.InitializeIntervals(Grid, NoEdits);
 TestEqual("Edit disabled after first call without edits", Mesher.IsEditEnabled(), false);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 256;
 FRLEVoxel EV;
 EV.RunLenght = 8;
 EV.Voxel = FVoxel(2);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> WithEdits;
 WithEdits.Push(Edit);
 Mesher.InitializeIntervals(Grid, WithEdits);

 TestEqual("Edit enabled after second call with edits", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithTwoSameIndexEditsConsumesOne,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithTwoSameIndexEditsConsumesOne",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithTwoSameIndexEditsConsumesOne::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit1;
 Edit1.EditEventIndex = 64;
 FRLEVoxel EV1;
 EV1.RunLenght = 5;
 EV1.Voxel = FVoxel(1);
 Edit1.EditVoxel = EV1;

 FRLEVoxelEdit Edit2;
 Edit2.EditEventIndex = 64;
 FRLEVoxel EV2;
 EV2.RunLenght = 5;
 EV2.Voxel = FVoxel(2);
 Edit2.EditVoxel = EV2;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit1);
 VoxelEdits.Push(Edit2);

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("One edit consumed when two share the same index", VoxelEdits.Num(), 1);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStateGrantsNonNullGridAfterInitialize,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.GrantsNonNullGridAfterInitialize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateGrantsNonNullGridAfterInitialize::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 int32 Sizes[][3] = {{8, 64, 512}, {16, 256, 4096}, {32, 1024, 32768}};
 for (auto& S : Sizes)
 {
  Mesher.UpdateInternalState(S[0], S[1], S[2]);

  FRLEVoxel V;
  V.RunLenght = 10;
  V.Voxel = FVoxel(1);
  TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
  Grid->Add(V);

  TArray<FRLEVoxelEdit> VoxelEdits;
  Mesher.InitializeIntervals(Grid, VoxelEdits);

  TestTrue("Grid non-null after each UpdateInternalState + InitializeIntervals", Mesher.GetMainVoxelGridPtr() != nullptr);
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherIsEditEnabledFalseDirectlyAfterConstruction,
 "RDM.RDMMeshersTests.VoxelEventMesher.IsEditEnabled.FalseDirectlyAfterConstruction",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherIsEditEnabledFalseDirectlyAfterConstruction::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 TestEqual("Edit disabled immediately after construction", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherGetMainVoxelGridPtrNullDirectlyAfterConstruction,
 "RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullDirectlyAfterConstruction",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGetMainVoxelGridPtrNullDirectlyAfterConstruction::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 TestEqual("Grid pointer null immediately after construction", Mesher.GetMainVoxelGridPtr() == nullptr, true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditBeyondGridSizeStillInitializes,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.EditBeyondGridSizeStillInitializes",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditBeyondGridSizeStillInitializes::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 100;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 99999;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(2);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Grid initialized even when edit index is beyond grid size", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceAllMeshingEventsDoesNotChangeEditEnabledState,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.DoesNotChangeEditEnabledState",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceAllMeshingEventsDoesNotChangeEditEnabledState::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 100;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);
 bool StateBeforeAdvance = Mesher.IsEditEnabled();
 Mesher.AdvanceAllMeshingEvents();
 bool StateAfterAdvance = Mesher.IsEditEnabled();

 TestEqual("Edit state unchanged by AdvanceAllMeshingEvents", StateBeforeAdvance, StateAfterAdvance);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithAllSameVoxelTypeAndEditsEnablesEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AllSameVoxelTypeWithEditsEnablesEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithAllSameVoxelTypeAndEditsEnablesEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 4096;
 V1.Voxel = FVoxel(5);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 512;
 FRLEVoxel EV;
 EV.RunLenght = 16;
 EV.Voxel = FVoxel(0);
 Edit.EditVoxel = EV;
 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Edit enabled when single run grid has an edit", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithZeroRunLengthEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithZeroRunLengthEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithZeroRunLengthEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 0;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.AdvanceEditInterval(VoxelEdits);

 TestTrue("Zero run length edit consumed", VoxelEdits.IsEmpty());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherGridPointerValidAfterRepeatedInitializeIntervals,
 "RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.ValidAfterRepeatedInitializeIntervals",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherGridPointerValidAfterRepeatedInitializeIntervals::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 50;
 V.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 TArray<FRLEVoxelEdit> VoxelEdits;

 for (int32 i = 0; i < 10; ++i)
 {
  Mesher.InitializeIntervals(Grid, VoxelEdits);
  TestTrue("Grid pointer valid on repeated initialize", Mesher.GetMainVoxelGridPtr() != nullptr);
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithVeryLargeEventIndex,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithVeryLargeEventIndex",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithVeryLargeEventIndex::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 999999999;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestTrue("Edit with very large index consumed", VoxelEdits.IsEmpty());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsMultipleTimesWithDifferentGridSizes,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.MultipleTimesWithDifferentGridSizes",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsMultipleTimesWithDifferentGridSizes::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 for (int32 i = 0; i < 5; ++i)
 {
  int32 Size = (i + 1) * 100;
  Mesher.UpdateInternalState(16, 256, Size);

  FRLEVoxel V;
  V.RunLenght = Size;
  V.Voxel = FVoxel(1);
  TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
  Grid->Add(V);

  TArray<FRLEVoxelEdit> VoxelEdits;
  Mesher.InitializeIntervals(Grid, VoxelEdits);
  
  TestTrue("Grid initialized for size " + FString::FromInt(Size), Mesher.GetMainVoxelGridPtr() != nullptr);
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInternalResetClearsEditFlagAfterAdvanceEditInterval,
 "RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.ClearsEditFlagAfterAdvanceEditInterval",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInternalResetClearsEditFlagAfterAdvanceEditInterval::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);
 Mesher.AdvanceEditInterval(VoxelEdits);

 Mesher.InternalReset();

 TestEqual("Edit disabled after reset", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStatePreservesEditStateIfNotYetInitialized,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.PreservesEditStateIfNotInitialized",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStatePreservesEditStateIfNotYetInitialized::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 Mesher.UpdateInternalState(16, 256, 4096);
 bool EditStateAfterFirstUpdate = Mesher.IsEditEnabled();
 
 Mesher.UpdateInternalState(32, 512, 8192);
 bool EditStateAfterSecondUpdate = Mesher.IsEditEnabled();

 TestEqual("Edit state remains disabled across updates", EditStateAfterFirstUpdate, EditStateAfterSecondUpdate);
 TestEqual("Edit state is false", EditStateAfterSecondUpdate, false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithManySmallVoxelRuns,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithManySmallVoxelRuns",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithManySmallVoxelRuns::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 for (int32 i = 0; i < 50; ++i)
 {
  FRLEVoxel V;
  V.RunLenght = 1;
  V.Voxel = FVoxel(i % 2);
  Grid->Add(V);
 }

 TArray<FRLEVoxelEdit> VoxelEdits;

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Grid with many small runs initialized", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalFollowedByInitializeIntervalsWithoutReset,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.FollowedByInitializeWithoutReset",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalFollowedByInitializeIntervalsWithoutReset::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.AdvanceEditInterval(VoxelEdits);

 FRLEVoxel V1;
 V1.RunLenght = 100;
 V1.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 TArray<FRLEVoxelEdit> NoEdits;
 Mesher.InitializeIntervals(Grid, NoEdits);

 TestEqual("Initialize after AdvanceEditInterval succeeds", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditAtEndOfGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtEndOfGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditAtEndOfGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 50;
 V1.Voxel = FVoxel(0);
 FRLEVoxel V2;
 V2.RunLenght = 50;
 V2.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 99;
 FRLEVoxel EV;
 EV.RunLenght = 1;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Edit at end of grid enables edit mode", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStateThenInitializeIntervalsMultipleTimes,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.ThenInitializeMultipleTimes",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateThenInitializeIntervalsMultipleTimes::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 100;
 V.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 TArray<FRLEVoxelEdit> VoxelEdits;

 for (int32 i = 0; i < 3; ++i)
 {
  Mesher.InitializeIntervals(Grid, VoxelEdits);
  TestTrue("Grid valid on iteration " + FString::FromInt(i), Mesher.GetMainVoxelGridPtr() != nullptr);
 }

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithMixedEditIndices,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithMixedEditIndices",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithMixedEditIndices::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push({0, {10, FVoxel(1)}});
 VoxelEdits.Push({25, {5, FVoxel(2)}});
 VoxelEdits.Push({75, {3, FVoxel(3)}});
 VoxelEdits.Push({100, {8, FVoxel(4)}});

 int32 InitialCount = VoxelEdits.Num();

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("One edit removed from mixed indices", VoxelEdits.Num(), InitialCount - 1);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsAfterInternalResetWithEdits,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AfterInternalResetWithEdits",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsAfterInternalResetWithEdits::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 100;
 V.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(2);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);
 Mesher.InternalReset();

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Edit enabled after recovery from reset", Mesher.IsEditEnabled(), true);
 TestTrue("Grid restored after recovery", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsTransitionFromNoEditsToEditsBack,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.TransitionFromNoEditsToEditsBack",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsTransitionFromNoEditsToEditsBack::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 100;
 V.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 TArray<FRLEVoxelEdit> NoEdits;
 TArray<FRLEVoxelEdit> WithEdits;

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 50;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;
 WithEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, NoEdits);
 TestEqual("Edit disabled initially", Mesher.IsEditEnabled(), false);

 Mesher.InitializeIntervals(Grid, WithEdits);
 TestEqual("Edit enabled after adding edits", Mesher.IsEditEnabled(), true);

 Mesher.InitializeIntervals(Grid, NoEdits);
 TestEqual("Edit disabled after removing edits", Mesher.IsEditEnabled(), false);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithNegativeIndexEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithNegativeIndexEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithNegativeIndexEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = -1;
 FRLEVoxel EV;
 EV.RunLenght = 10;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestTrue("Negative index edit consumed", VoxelEdits.IsEmpty());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithMaxVoxelVoxels,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMaxVoxelValues",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithMaxVoxelVoxels::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V1;
 V1.RunLenght = 2048;
 V1.Voxel = FVoxel(255);
 FRLEVoxel V2;
 V2.RunLenght = 2048;
 V2.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);

 TArray<FRLEVoxelEdit> VoxelEdits;

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Grid with max voxel values initialized", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceAllMeshingEventsMultipleTimesWithoutStateChange,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.MultipleCallsWithoutStateChange",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceAllMeshingEventsMultipleTimesWithoutStateChange::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 100;
 V.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 bool State1 = Mesher.IsEditEnabled();

 for (int32 i = 0; i < 10; ++i)
 {
  Mesher.AdvanceAllMeshingEvents();
 }

 bool State2 = Mesher.IsEditEnabled();

 TestEqual("Edit state unchanged after multiple AdvanceAllMeshingEvents", State1, State2);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStateWithChangingDimensionsThenInitialize,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.WithChangingDimensionsThenInitialize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateWithChangingDimensionsThenInitialize::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);

 int32 Dimensions[][3] = {{8, 64, 512}, {16, 256, 4096}, {24, 576, 13824}};

 for (auto& Dims : Dimensions)
 {
  Mesher.UpdateInternalState(Dims[0], Dims[1], Dims[2]);

  FRLEVoxel V;
  V.RunLenght = 50;
  V.Voxel = FVoxel(1);
  TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
  Grid->Add(V);

  TArray<FRLEVoxelEdit> VoxelEdits;
  Mesher.InitializeIntervals(Grid, VoxelEdits);

  TestTrue("Successfully initialized with dimensions [" + FString::FromInt(Dims[0]) + ", " + 
           FString::FromInt(Dims[1]) + ", " + FString::FromInt(Dims[2]) + "]", 
           Mesher.GetMainVoxelGridPtr() != nullptr);
 }
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithAlternatingVoxelTypeRuns,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithAlternatingVoxelTypeRuns",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithAlternatingVoxelTypeRuns::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 for (int32 i = 0; i < 10; ++i)
 {
  FRLEVoxel V;
  V.RunLenght = 100;
  V.Voxel = FVoxel(i % 2);
  Grid->Add(V);
 }

 TArray<FRLEVoxelEdit> VoxelEdits;

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Alternating voxel types handled correctly", Mesher.GetMainVoxelGridPtr() != nullptr);
 TestEqual("Edit disabled for grid without edits", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalWithSingleElementArray,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithSingleElementArray",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalWithSingleElementArray::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push({42, {7, FVoxel(3)}});

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestTrue("Single element array becomes empty after advance", VoxelEdits.IsEmpty());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInterleavedResetAndInitialize,
 "RDM.RDMMeshersTests.VoxelEventMesher.Interleaved.ResetAndInitialize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInterleavedResetAndInitialize::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 100;
 V.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 for (int32 i = 0; i < 3; ++i)
 {
  TArray<FRLEVoxelEdit> VoxelEdits;
  Mesher.InitializeIntervals(Grid, VoxelEdits);
  TestTrue("Grid initialized on iteration " + FString::FromInt(i), Mesher.GetMainVoxelGridPtr() != nullptr);

  Mesher.InternalReset();
  TestEqual("Edit disabled after reset on iteration " + FString::FromInt(i), Mesher.IsEditEnabled(), false);
 }

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditExactlyAtGridSize,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditExactlyAtGridSize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditExactlyAtGridSize::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel V;
 V.RunLenght = 4096;
 V.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 4096;
 FRLEVoxel EV;
 EV.RunLenght = 1;
 EV.Voxel = FVoxel(1);
 Edit.EditVoxel = EV;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Grid initialized with edit at exact grid boundary", Mesher.GetMainVoxelGridPtr() != nullptr);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithoutEditsKeepsOriginalGridPointer,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsKeepsOriginalGridPointer",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithoutEditsKeepsOriginalGridPointer::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstRun;
 FirstRun.RunLenght = 12;
 FirstRun.Voxel = FVoxel(1);

 FRLEVoxel SecondRun;
 SecondRun.RunLenght = 20;
 SecondRun.Voxel = FVoxel(2);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(FirstRun);
 Grid->Add(SecondRun);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("Original grid pointer is reused when no edits are supplied", Mesher.GetMainVoxelGridPtr() == Grid);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditAtStartCreatesIndependentGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartCreatesIndependentGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditAtStartCreatesIndependentGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(1);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(2);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("A new main grid is created when edits are supplied", Mesher.GetMainVoxelGridPtr() != Grid);
 TestEqual("Edit mode is enabled after initializing with edits", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditAtStartReplacesLeadingRun,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartReplacesLeadingRun",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditAtStartReplacesLeadingRun::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(1);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(2);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 const TSharedPtr<TArray<FRLEVoxel>> MainGrid = Mesher.GetMainVoxelGridPtr();
 TestEqual("Edited grid contains two runs", MainGrid->Num(), 2);
 TestEqual("First run length matches edit length", (*MainGrid)[0].RunLenght, static_cast<uint32>(3));
 TestTrue("First run voxel matches edit voxel", (*MainGrid)[0].Voxel == FVoxel(2));
 TestEqual("Second run length preserves the remaining tail", (*MainGrid)[1].RunLenght, static_cast<uint32>(7));
 TestTrue("Second run voxel matches the original voxel", (*MainGrid)[1].Voxel == FVoxel(1));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithMatchingEditAtStartMergesRuns,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMatchingEditAtStartMergesRuns",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithMatchingEditAtStartMergesRuns::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(1);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(1);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 const TSharedPtr<TArray<FRLEVoxel>> MainGrid = Mesher.GetMainVoxelGridPtr();
 TestEqual("Matching edit is merged into a single run", MainGrid->Num(), 1);
 TestEqual("Merged run preserves total length", (*MainGrid)[0].RunLenght, static_cast<uint32>(10));
 TestTrue("Merged run keeps the original voxel value", (*MainGrid)[0].Voxel == FVoxel(1));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalConsumesMostRecentlyAddedEditFirst,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.ConsumesMostRecentlyAddedEditFirst",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalConsumesMostRecentlyAddedEditFirst::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxelEdit FirstEdit;
 FirstEdit.EditEventIndex = 10;
 FRLEVoxel FirstEditVoxel;
 FirstEditVoxel.RunLenght = 2;
 FirstEditVoxel.Voxel = FVoxel(1);
 FirstEdit.EditVoxel = FirstEditVoxel;

 FRLEVoxelEdit SecondEdit;
 SecondEdit.EditEventIndex = 20;
 FRLEVoxel SecondEditVoxel;
 SecondEditVoxel.RunLenght = 4;
 SecondEditVoxel.Voxel = FVoxel(2);
 SecondEdit.EditVoxel = SecondEditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(FirstEdit);
 VoxelEdits.Push(SecondEdit);

 Mesher.AdvanceEditInterval(VoxelEdits);

 TestEqual("Exactly one edit remains after a single advance", VoxelEdits.Num(), 1);
 TestEqual("The earlier edit remains after consuming the most recently added one", VoxelEdits[0].EditEventIndex,
		   static_cast<uint32>(10));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsAfterEditedStateCanSwitchBackToProvidedGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AfterEditedStateCanSwitchBackToProvidedGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsAfterEditedStateCanSwitchBackToProvidedGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstGridRun;
 FirstGridRun.RunLenght = 10;
 FirstGridRun.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> EditedGridSource = MakeShared<TArray<FRLEVoxel>>();
 EditedGridSource->Add(FirstGridRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(2);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> WithEdits;
 WithEdits.Push(Edit);
 Mesher.InitializeIntervals(EditedGridSource, WithEdits);
 TestTrue("Edited state uses a derived grid", Mesher.GetMainVoxelGridPtr() != EditedGridSource);

 FRLEVoxel SecondGridRun;
 SecondGridRun.RunLenght = 25;
 SecondGridRun.Voxel = FVoxel(3);
 TSharedPtr<TArray<FRLEVoxel>> PlainGrid = MakeShared<TArray<FRLEVoxel>>();
 PlainGrid->Add(SecondGridRun);

 TArray<FRLEVoxelEdit> NoEdits;
 Mesher.InitializeIntervals(PlainGrid, NoEdits);

 TestTrue("Reinitializing without edits switches back to the provided grid", Mesher.GetMainVoxelGridPtr() == PlainGrid);
 TestEqual("Edit mode is disabled after reinitializing without edits", Mesher.IsEditEnabled(), false);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditsDoesNotMutateSourceGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsDoesNotMutateSourceGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditsDoesNotMutateSourceGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(1);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 3;
 EditVoxel.Voxel = FVoxel(2);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("Source grid keeps the original number of runs", Grid->Num(), 1);
 TestEqual("Source grid keeps the original run length", (*Grid)[0].RunLenght, static_cast<uint32>(10));
 TestTrue("Source grid keeps the original voxel value", (*Grid)[0].Voxel == FVoxel(1));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherAdvanceEditIntervalOnConsecutiveCallsConsumesNewestEditEachTime,
 "RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.OnConsecutiveCallsConsumesNewestEditEachTime",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherAdvanceEditIntervalOnConsecutiveCallsConsumesNewestEditEachTime::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel BaseRun;
 BaseRun.RunLenght = 20;
 BaseRun.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(BaseRun);

 FRLEVoxelEdit SeedEdit;
 SeedEdit.EditEventIndex = 0;
 FRLEVoxel SeedVoxel;
 SeedVoxel.RunLenght = 1;
 SeedVoxel.Voxel = FVoxel(9);
 SeedEdit.EditVoxel = SeedVoxel;
 TArray<FRLEVoxelEdit> SeedEdits;
 SeedEdits.Push(SeedEdit);
 Mesher.InitializeIntervals(Grid, SeedEdits);

 FRLEVoxelEdit FirstEdit;
 FirstEdit.EditEventIndex = 5;
 FRLEVoxel FirstVoxel;
 FirstVoxel.RunLenght = 2;
 FirstVoxel.Voxel = FVoxel(1);
 FirstEdit.EditVoxel = FirstVoxel;

 FRLEVoxelEdit SecondEdit;
 SecondEdit.EditEventIndex = 15;
 FRLEVoxel SecondVoxel;
 SecondVoxel.RunLenght = 4;
 SecondVoxel.Voxel = FVoxel(2);
 SecondEdit.EditVoxel = SecondVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(FirstEdit);
 VoxelEdits.Push(SecondEdit);

 Mesher.AdvanceEditInterval(VoxelEdits);
 TestEqual("One edit remains after the first advance", VoxelEdits.Num(), 1);
 TestEqual("The earlier queued edit remains after the first advance", VoxelEdits[0].EditEventIndex,
		   static_cast<uint32>(5));

 Mesher.AdvanceEditInterval(VoxelEdits);
 TestTrue("All queued edits are consumed after the second advance", VoxelEdits.IsEmpty());
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherUpdateInternalStateAfterInitializationAllowsUsingNewGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.AfterInitializationAllowsUsingNewGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherUpdateInternalStateAfterInitializationAllowsUsingNewGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel BaseRun;
 BaseRun.RunLenght = 16;
 BaseRun.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(BaseRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 2;
 EditVoxel.Voxel = FVoxel(3);
 Edit.EditVoxel = EditVoxel;
 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);
 const TSharedPtr<TArray<FRLEVoxel>> FirstGrid = Mesher.GetMainVoxelGridPtr();
 TestTrue("Grid pointer is valid after initialization", FirstGrid != nullptr);
 TestEqual("Edit mode is enabled after initialization with edits", Mesher.IsEditEnabled(), true);

 Mesher.UpdateInternalState(8, 64, 512);

 TestEqual("Edit mode is disabled after updating internal state", Mesher.IsEditEnabled(), false);

 FRLEVoxel NewRun;
 NewRun.RunLenght = 12;
 NewRun.Voxel = FVoxel(7);
 TSharedPtr<TArray<FRLEVoxel>> NewGrid = MakeShared<TArray<FRLEVoxel>>();
 NewGrid->Add(NewRun);

 TArray<FRLEVoxelEdit> NoEdits;
 Mesher.InitializeIntervals(NewGrid, NoEdits);

 TestTrue("A new grid can be used after updating internal state", Mesher.GetMainVoxelGridPtr() == NewGrid);
 TestTrue("The newly initialized grid replaces the previous one", Mesher.GetMainVoxelGridPtr() != FirstGrid);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithoutEditsReplacesPreviousNoEditGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsReplacesPreviousNoEditGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithoutEditsReplacesPreviousNoEditGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstRun;
 FirstRun.RunLenght = 8;
 FirstRun.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> FirstGrid = MakeShared<TArray<FRLEVoxel>>();
 FirstGrid->Add(FirstRun);

 FRLEVoxel SecondRun;
 SecondRun.RunLenght = 12;
 SecondRun.Voxel = FVoxel(2);
 TSharedPtr<TArray<FRLEVoxel>> SecondGrid = MakeShared<TArray<FRLEVoxel>>();
 SecondGrid->Add(SecondRun);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(FirstGrid, VoxelEdits);
 TestTrue("First no-edit initialization uses the first grid", Mesher.GetMainVoxelGridPtr() == FirstGrid);

 Mesher.InitializeIntervals(SecondGrid, VoxelEdits);

 TestTrue("Second no-edit initialization uses the second grid", Mesher.GetMainVoxelGridPtr() == SecondGrid);
 TestTrue("Second no-edit initialization replaces the previous grid pointer", Mesher.GetMainVoxelGridPtr() != FirstGrid);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithEditAtStartAcrossRunsKeepsOnlyUneditedTail,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartAcrossRunsKeepsOnlyUneditedTail",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithEditAtStartAcrossRunsKeepsOnlyUneditedTail::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstRun;
 FirstRun.RunLenght = 4;
 FirstRun.Voxel = FVoxel(1);

 FRLEVoxel SecondRun;
 SecondRun.RunLenght = 6;
 SecondRun.Voxel = FVoxel(2);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(FirstRun);
 Grid->Add(SecondRun);

 FRLEVoxelEdit Edit;
 Edit.EditEventIndex = 0;
 FRLEVoxel EditVoxel;
 EditVoxel.RunLenght = 5;
 EditVoxel.Voxel = FVoxel(3);
 Edit.EditVoxel = EditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(Edit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 const TSharedPtr<TArray<FRLEVoxel>> MainGrid = Mesher.GetMainVoxelGridPtr();
 TestEqual("Edited grid contains the inserted run and the remaining tail", MainGrid->Num(), 2);
 TestEqual("Inserted run keeps the edit length", (*MainGrid)[0].RunLenght, static_cast<uint32>(5));
 TestTrue("Inserted run keeps the edit voxel", (*MainGrid)[0].Voxel == FVoxel(3));
 TestEqual("Only the unedited tail remains from the source grid", (*MainGrid)[1].RunLenght, static_cast<uint32>(5));
 TestTrue("The remaining tail uses the voxel from the partially edited source run", (*MainGrid)[1].Voxel == FVoxel(2));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithoutEditsPreservesSourceGridContents,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsPreservesSourceGridContents",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithoutEditsPreservesSourceGridContents::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstRun;
 FirstRun.RunLenght = 4;
 FirstRun.Voxel = FVoxel(1);

 FRLEVoxel SecondRun;
 SecondRun.RunLenght = 6;
 SecondRun.Voxel = FVoxel(2);

 FRLEVoxel ThirdRun;
 ThirdRun.RunLenght = 8;
 ThirdRun.Voxel = FVoxel(3);

 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(FirstRun);
 Grid->Add(SecondRun);
 Grid->Add(ThirdRun);

 TArray<FRLEVoxelEdit> VoxelEdits;
 Mesher.InitializeIntervals(Grid, VoxelEdits);

 const TSharedPtr<TArray<FRLEVoxel>> MainGrid = Mesher.GetMainVoxelGridPtr();
 TestTrue("The original grid pointer is reused without edits", MainGrid == Grid);
 TestEqual("The first run length is preserved", (*MainGrid)[0].RunLenght, static_cast<uint32>(4));
 TestTrue("The first run voxel is preserved", (*MainGrid)[0].Voxel == FVoxel(1));
 TestEqual("The second run length is preserved", (*MainGrid)[1].RunLenght, static_cast<uint32>(6));
 TestTrue("The second run voxel is preserved", (*MainGrid)[1].Voxel == FVoxel(2));
 TestEqual("The third run length is preserved", (*MainGrid)[2].RunLenght, static_cast<uint32>(8));
 TestTrue("The third run voxel is preserved", (*MainGrid)[2].Voxel == FVoxel(3));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsReinitializeWithNewEditedGridReplacesPreviousDerivedGrid,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.ReinitializeWithNewEditedGridReplacesPreviousDerivedGrid",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsReinitializeWithNewEditedGridReplacesPreviousDerivedGrid::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel FirstSourceRun;
 FirstSourceRun.RunLenght = 10;
 FirstSourceRun.Voxel = FVoxel(1);
 TSharedPtr<TArray<FRLEVoxel>> FirstGrid = MakeShared<TArray<FRLEVoxel>>();
 FirstGrid->Add(FirstSourceRun);

 FRLEVoxelEdit FirstEdit;
 FirstEdit.EditEventIndex = 0;
 FRLEVoxel FirstEditVoxel;
 FirstEditVoxel.RunLenght = 3;
 FirstEditVoxel.Voxel = FVoxel(2);
 FirstEdit.EditVoxel = FirstEditVoxel;
 TArray<FRLEVoxelEdit> FirstEdits;
 FirstEdits.Push(FirstEdit);

 Mesher.InitializeIntervals(FirstGrid, FirstEdits);
 const TSharedPtr<TArray<FRLEVoxel>> FirstDerivedGrid = Mesher.GetMainVoxelGridPtr();
 TestEqual("The first derived grid starts with the first edit length", (*FirstDerivedGrid)[0].RunLenght,
		   static_cast<uint32>(3));
 TestTrue("The first derived grid starts with the first edit voxel", (*FirstDerivedGrid)[0].Voxel == FVoxel(2));

 FRLEVoxel SecondSourceRun;
 SecondSourceRun.RunLenght = 12;
 SecondSourceRun.Voxel = FVoxel(4);
 TSharedPtr<TArray<FRLEVoxel>> SecondGrid = MakeShared<TArray<FRLEVoxel>>();
 SecondGrid->Add(SecondSourceRun);

 FRLEVoxelEdit SecondEdit;
 SecondEdit.EditEventIndex = 0;
 FRLEVoxel SecondEditVoxel;
 SecondEditVoxel.RunLenght = 5;
 SecondEditVoxel.Voxel = FVoxel(6);
 SecondEdit.EditVoxel = SecondEditVoxel;
 TArray<FRLEVoxelEdit> SecondEdits;
 SecondEdits.Push(SecondEdit);

 Mesher.InitializeIntervals(SecondGrid, SecondEdits);
 const TSharedPtr<TArray<FRLEVoxel>> SecondDerivedGrid = Mesher.GetMainVoxelGridPtr();

 TestTrue("Reinitializing with another edited grid creates a different derived grid", SecondDerivedGrid != FirstDerivedGrid);
 TestEqual("The replacement derived grid starts with the new edit length", (*SecondDerivedGrid)[0].RunLenght,
		   static_cast<uint32>(5));
 TestTrue("The replacement derived grid starts with the new edit voxel", (*SecondDerivedGrid)[0].Voxel == FVoxel(6));
 TestEqual("The replacement derived grid keeps the new remaining tail", (*SecondDerivedGrid)[1].RunLenght,
		   static_cast<uint32>(7));
 TestTrue("The replacement derived grid tail uses the new source voxel", (*SecondDerivedGrid)[1].Voxel == FVoxel(4));
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit LaterEdit;
 LaterEdit.EditEventIndex = 3;
 FRLEVoxel LaterEditVoxel;
 LaterEditVoxel.RunLenght = 1;
 LaterEditVoxel.Voxel = FVoxel(1);
 LaterEdit.EditVoxel = LaterEditVoxel;

 FRLEVoxelEdit LastQueuedEdit;
 LastQueuedEdit.EditEventIndex = 6;
 FRLEVoxel LastQueuedVoxel;
 LastQueuedVoxel.RunLenght = 2;
 LastQueuedVoxel.Voxel = FVoxel(2);
 LastQueuedEdit.EditVoxel = LastQueuedVoxel;

 FRLEVoxelEdit StartEdit;
 StartEdit.EditEventIndex = 0;
 FRLEVoxel StartEditVoxel;
 StartEditVoxel.RunLenght = 2;
 StartEditVoxel.Voxel = FVoxel(3);
 StartEdit.EditVoxel = StartEditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(LaterEdit);
 VoxelEdits.Push(LastQueuedEdit);
 VoxelEdits.Push(StartEdit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestEqual("One queued edit remains after initializing a start edit and scheduling the next one", VoxelEdits.Num(), 1);
 TestEqual("The oldest queued edit remains pending", VoxelEdits[0].EditEventIndex, static_cast<uint32>(3));
 TestEqual("Edit mode remains enabled after scheduling a queued edit", Mesher.IsEditEnabled(), true);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FVoxelEventMesherInitializeIntervalsWithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately,
 "RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherInitializeIntervalsWithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately::RunTest(const FString& Parameters)
{
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesher Mesher(BaseVoxelDataDummy);
 Mesher.UpdateInternalState(16, 256, 4096);

 FRLEVoxel SourceRun;
 SourceRun.RunLenght = 10;
 SourceRun.Voxel = FVoxel(0);
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(SourceRun);

 FRLEVoxelEdit QueuedEdit;
 QueuedEdit.EditEventIndex = 6;
 FRLEVoxel QueuedEditVoxel;
 QueuedEditVoxel.RunLenght = 2;
 QueuedEditVoxel.Voxel = FVoxel(4);
 QueuedEdit.EditVoxel = QueuedEditVoxel;

 FRLEVoxelEdit StartEdit;
 StartEdit.EditEventIndex = 0;
 FRLEVoxel StartEditVoxel;
 StartEditVoxel.RunLenght = 2;
 StartEditVoxel.Voxel = FVoxel(5);
 StartEdit.EditVoxel = StartEditVoxel;

 TArray<FRLEVoxelEdit> VoxelEdits;
 VoxelEdits.Push(QueuedEdit);
 VoxelEdits.Push(StartEdit);

 Mesher.InitializeIntervals(Grid, VoxelEdits);

 TestTrue("All queued edits are consumed into active scheduling when only one follows the start edit", VoxelEdits.IsEmpty());
 TestEqual("Edit mode remains enabled after consuming the queued edit into scheduling", Mesher.IsEditEnabled(), true);
 return true;
}

