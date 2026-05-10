#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "VoxelMesher/VoxelEventMesher.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelMesher/MeshingUtil/RLEVoxelEdit.h"

// AdvanceEditInterval Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_AdvanceEditIntervalWithEmptyEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithEmptyEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithEmptyEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithSingleEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithSingleEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithSingleEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithMultipleEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithMultipleEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithMultipleEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateUpdatesParameters,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.UpdatesParameters",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateUpdatesParameters::RunTest(const FString& Parameters)
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



// InitializeIntervals Tests
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithoutEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithoutEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditsAtStart,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsAtStart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditsAtStart::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditsAtMiddle,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsAtMiddle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditsAtMiddle::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsCreatesMainVoxelGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.CreatesMainVoxelGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsCreatesMainVoxelGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InternalResetDisablesEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.DisablesEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InternalResetDisablesEdits::RunTest(const FString& Parameters)
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


// Constructor Tests


// AdvanceEditInterval with various scenarios Tests


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithLargeGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithLargeGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithLargeGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithMultipleGridRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMultipleGridRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithMultipleGridRuns::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithLargeRunLength,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithLargeRunLength",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithLargeRunLength::RunTest(const FString& Parameters)
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


// EditVoxelGrid Tests

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_EditVoxelGridWithEditsAtStart,
	"RDM.RDMMeshersTests.VoxelEventMesher.EditVoxelGrid.WithEditsAtStart",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_EditVoxelGridWithEditsAtStart::RunTest(const FString& Parameters)
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



// AdvanceAllMeshingEvents Tests


// ConvertVirtualFacesToMesh Tests



// CreateVirtualVoxelFacesInLShape Tests


// TraverseYDirection Tests


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_StateConsistencyAcrossOperations,
	"RDM.RDMMeshersTests.VoxelEventMesher.StateConsistency.AcrossOperations",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_StateConsistencyAcrossOperations::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_LargeScaleGridProcessing,
	"RDM.RDMMeshersTests.VoxelEventMesher.LargeScale.GridProcessing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_LargeScaleGridProcessing::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_ReinitializeAfterNoEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.Reinitialize.AfterNoEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_ReinitializeAfterNoEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_ReinitializeAfterEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.Reinitialize.AfterEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_ReinitializeAfterEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalDrainsAllEditsSequentially,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.DrainsAllEditsSequentially",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalDrainsAllEditsSequentially::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_IsEditEnabledFalseBeforeInitializeIntervals,
	"RDM.RDMMeshersTests.VoxelEventMesher.IsEditEnabled.FalseBeforeInitializeIntervals",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_IsEditEnabledFalseBeforeInitializeIntervals::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	TestEqual("EditEnabled false before InitializeIntervals", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_GetMainVoxelGridPtrNullBeforeInitialize,
	"RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullBeforeInitialize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_GetMainVoxelGridPtrNullBeforeInitialize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);
	Mesher.UpdateInternalState(16, 256, 4096);

	TestEqual("MainVoxelGridPtr null before InitializeIntervals", Mesher.GetMainVoxelGridPtr() == nullptr, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsOverwritesPreviousGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.OverwritesPreviousGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsOverwritesPreviousGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalAfterInternalReset,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.AfterInternalReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalAfterInternalReset::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsEditAtExactRunBoundary,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.EditAtExactRunBoundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsEditAtExactRunBoundary::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalSingleEditBecomesDisabled,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.SingleEditBecomesDisabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalSingleEditBecomesDisabled::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateThenImmediatelyReset,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.ThenImmediatelyReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateThenImmediatelyReset::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithZeroRunLengthVoxel,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithZeroRunLengthVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithZeroRunLengthVoxel::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_EditEnabledTrueWithEditsAtPositionZero,
	"RDM.RDMMeshersTests.VoxelEventMesher.EditEnabled.TrueWithEditsAtPositionZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_EditEnabledTrueWithEditsAtPositionZero::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalMultipleEditsReducesCountByOne,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.MultipleEditsReducesCountByOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalMultipleEditsReducesCountByOne::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorReinitializeFromNoEditsToEditsTogglesEditEnabled,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.ReinitializeFromNoEditsToEditsTogglesEditEnabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorReinitializeFromNoEditsToEditsTogglesEditEnabled::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorReinitializeFromEditsToNoEditsDisablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.ReinitializeFromEditsToNoEditsDisablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorReinitializeFromEditsToNoEditsDisablesEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorAdvanceEditIntervalThreeEditsLeavesTwo,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.AdvanceEditIntervalThreeEditsLeavesTwo",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorAdvanceEditIntervalThreeEditsLeavesTwo::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorAdvanceEditIntervalWithNoEditsDisablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.AdvanceEditIntervalWithNoEditsDisablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorAdvanceEditIntervalWithNoEditsDisablesEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorInitializeIntervalsWithEditAtLastVoxelEnablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InitializeIntervalsWithEditAtLastVoxelEnablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorInitializeIntervalsWithEditAtLastVoxelEnablesEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorUpdateInternalStateWithArbitraryValuesAllowsInitialization,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.UpdateInternalStateWithArbitraryValuesAllowsInitialization",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorUpdateInternalStateWithArbitraryValuesAllowsInitialization::RunTest(
	const FString& Parameters)
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
	FVoxelEventMesher_BehaviorInternalResetAfterEditedInitializationCanRecover,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InternalResetAfterEditedInitializationCanRecover",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorInternalResetAfterEditedInitializationCanRecover::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_BehaviorInitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled,
	"RDM.RDMMeshersTests.VoxelEventMesher.Behavior.InitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_BehaviorInitializeIntervalsWithTwoEditsAtSameIndexKeepsEditEnabled::RunTest(
	const FString& Parameters)
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
	FVoxelEventMesher_GetMainVoxelGridPtrNullAfterInternalReset,
	"RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullAfterInternalReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_GetMainVoxelGridPtrNullAfterInternalReset::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalConsumesExactlyOneEditPerCall,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.ConsumesExactlyOneEditPerCall",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalConsumesExactlyOneEditPerCall::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalCalledUntilEmptyDisablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.CalledUntilEmptyDisablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalCalledUntilEmptyDisablesEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithNoRunsInGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithNoRunsInGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithNoRunsInGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceAllMeshingEventsAfterInternalReset,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.AfterInternalReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceAllMeshingEventsAfterInternalReset::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsSecondCallWithEditsAfterFirstWithoutEnablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.SecondCallWithEditsAfterFirstWithoutEnablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsSecondCallWithEditsAfterFirstWithoutEnablesEdit::RunTest(
	const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithTwoSameIndexEditsConsumesOne,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithTwoSameIndexEditsConsumesOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithTwoSameIndexEditsConsumesOne::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateGrantsNonNullGridAfterInitialize,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.GrantsNonNullGridAfterInitialize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateGrantsNonNullGridAfterInitialize::RunTest(const FString& Parameters)
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

		TestTrue("Grid non-null after each UpdateInternalState + InitializeIntervals",
		         Mesher.GetMainVoxelGridPtr() != nullptr);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_IsEditEnabledFalseDirectlyAfterConstruction,
	"RDM.RDMMeshersTests.VoxelEventMesher.IsEditEnabled.FalseDirectlyAfterConstruction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_IsEditEnabledFalseDirectlyAfterConstruction::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	TestEqual("Edit disabled immediately after construction", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_GetMainVoxelGridPtrNullDirectlyAfterConstruction,
	"RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.NullDirectlyAfterConstruction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_GetMainVoxelGridPtrNullDirectlyAfterConstruction::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesher Mesher(BaseVoxelDataDummy);

	TestEqual("Grid pointer null immediately after construction", Mesher.GetMainVoxelGridPtr() == nullptr, true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsEditBeyondGridSizeStillInitializes,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.EditBeyondGridSizeStillInitializes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsEditBeyondGridSizeStillInitializes::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceAllMeshingEventsDoesNotChangeEditEnabledState,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.DoesNotChangeEditEnabledState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceAllMeshingEventsDoesNotChangeEditEnabledState::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsAllSameVoxelTypeWithEditsEnablesEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AllSameVoxelTypeWithEditsEnablesEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsAllSameVoxelTypeWithEditsEnablesEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithZeroRunLengthEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithZeroRunLengthEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithZeroRunLengthEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_GetMainVoxelGridPtrValidAfterRepeatedInitializeIntervals,
	"RDM.RDMMeshersTests.VoxelEventMesher.GetMainVoxelGridPtr.ValidAfterRepeatedInitializeIntervals",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_GetMainVoxelGridPtrValidAfterRepeatedInitializeIntervals::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithVeryLargeEventIndex,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithVeryLargeEventIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithVeryLargeEventIndex::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsMultipleTimesWithDifferentGridSizes,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.MultipleTimesWithDifferentGridSizes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsMultipleTimesWithDifferentGridSizes::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InternalResetClearsEditFlagAfterAdvanceEditInterval,
	"RDM.RDMMeshersTests.VoxelEventMesher.InternalReset.ClearsEditFlagAfterAdvanceEditInterval",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InternalResetClearsEditFlagAfterAdvanceEditInterval::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStatePreservesEditStateIfNotInitialized,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.PreservesEditStateIfNotInitialized",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStatePreservesEditStateIfNotInitialized::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithManySmallVoxelRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithManySmallVoxelRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithManySmallVoxelRuns::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalFollowedByInitializeWithoutReset,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.FollowedByInitializeWithoutReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalFollowedByInitializeWithoutReset::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditAtEndOfGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtEndOfGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditAtEndOfGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateThenInitializeMultipleTimes,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.ThenInitializeMultipleTimes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateThenInitializeMultipleTimes::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithMixedEditIndices,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithMixedEditIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithMixedEditIndices::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsAfterInternalResetWithEdits,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AfterInternalResetWithEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsAfterInternalResetWithEdits::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsTransitionFromNoEditsToEditsBack,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.TransitionFromNoEditsToEditsBack",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsTransitionFromNoEditsToEditsBack::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithNegativeIndexEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithNegativeIndexEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithNegativeIndexEdit::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithMaxVoxelValues,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMaxVoxelValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithMaxVoxelValues::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceAllMeshingEventsMultipleCallsWithoutStateChange,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceAllMeshingEvents.MultipleCallsWithoutStateChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceAllMeshingEventsMultipleCallsWithoutStateChange::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateWithChangingDimensionsThenInitialize,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.WithChangingDimensionsThenInitialize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateWithChangingDimensionsThenInitialize::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithAlternatingVoxelTypeRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithAlternatingVoxelTypeRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithAlternatingVoxelTypeRuns::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalWithSingleElementArray,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.WithSingleElementArray",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalWithSingleElementArray::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InterleavedResetAndInitialize,
	"RDM.RDMMeshersTests.VoxelEventMesher.Interleaved.ResetAndInitialize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InterleavedResetAndInitialize::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditExactlyAtGridSize,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditExactlyAtGridSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditExactlyAtGridSize::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithoutEditsKeepsOriginalGridPointer,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsKeepsOriginalGridPointer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithoutEditsKeepsOriginalGridPointer::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditAtStartCreatesIndependentGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartCreatesIndependentGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditAtStartCreatesIndependentGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithEditAtStartReplacesLeadingRun,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartReplacesLeadingRun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditAtStartReplacesLeadingRun::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithMatchingEditAtStartMergesRuns,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithMatchingEditAtStartMergesRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithMatchingEditAtStartMergesRuns::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalConsumesMostRecentlyAddedEditFirst,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.ConsumesMostRecentlyAddedEditFirst",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalConsumesMostRecentlyAddedEditFirst::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsAfterEditedStateCanSwitchBackToProvidedGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.AfterEditedStateCanSwitchBackToProvidedGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsAfterEditedStateCanSwitchBackToProvidedGrid::RunTest(
	const FString& Parameters)
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

	TestTrue("Reinitializing without edits switches back to the provided grid",
	         Mesher.GetMainVoxelGridPtr() == PlainGrid);
	TestEqual("Edit mode is disabled after reinitializing without edits", Mesher.IsEditEnabled(), false);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithEditsDoesNotMutateSourceGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditsDoesNotMutateSourceGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditsDoesNotMutateSourceGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_AdvanceEditIntervalOnConsecutiveCallsConsumesNewestEditEachTime,
	"RDM.RDMMeshersTests.VoxelEventMesher.AdvanceEditInterval.OnConsecutiveCallsConsumesNewestEditEachTime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_AdvanceEditIntervalOnConsecutiveCallsConsumesNewestEditEachTime::RunTest(
	const FString& Parameters)
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
	FVoxelEventMesher_UpdateInternalStateAfterInitializationAllowsUsingNewGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.UpdateInternalState.AfterInitializationAllowsUsingNewGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_UpdateInternalStateAfterInitializationAllowsUsingNewGrid::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsWithoutEditsReplacesPreviousNoEditGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsReplacesPreviousNoEditGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithoutEditsReplacesPreviousNoEditGrid::RunTest(const FString& Parameters)
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
	TestTrue("Second no-edit initialization replaces the previous grid pointer",
	         Mesher.GetMainVoxelGridPtr() != FirstGrid);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithEditAtStartAcrossRunsKeepsOnlyUneditedTail,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithEditAtStartAcrossRunsKeepsOnlyUneditedTail",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithEditAtStartAcrossRunsKeepsOnlyUneditedTail::RunTest(
	const FString& Parameters)
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
	TestTrue("The remaining tail uses the voxel from the partially edited source run",
	         (*MainGrid)[1].Voxel == FVoxel(2));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithoutEditsPreservesSourceGridContents,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithoutEditsPreservesSourceGridContents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithoutEditsPreservesSourceGridContents::RunTest(const FString& Parameters)
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
	FVoxelEventMesher_InitializeIntervalsReinitializeWithNewEditedGridReplacesPreviousDerivedGrid,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.ReinitializeWithNewEditedGridReplacesPreviousDerivedGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsReinitializeWithNewEditedGridReplacesPreviousDerivedGrid::RunTest(
	const FString& Parameters)
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

	TestTrue("Reinitializing with another edited grid creates a different derived grid",
	         SecondDerivedGrid != FirstDerivedGrid);
	TestEqual("The replacement derived grid starts with the new edit length", (*SecondDerivedGrid)[0].RunLenght,
	          static_cast<uint32>(5));
	TestTrue("The replacement derived grid starts with the new edit voxel", (*SecondDerivedGrid)[0].Voxel == FVoxel(6));
	TestEqual("The replacement derived grid keeps the new remaining tail", (*SecondDerivedGrid)[1].RunLenght,
	          static_cast<uint32>(7));
	TestTrue("The replacement derived grid tail uses the new source voxel", (*SecondDerivedGrid)[1].Voxel == FVoxel(4));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithStartEditAndAdditionalQueuedEditsLeavesOnePendingEdit::RunTest(
	const FString& Parameters)
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

	TestEqual("One queued edit remains after initializing a start edit and scheduling the next one", VoxelEdits.Num(),
	          1);
	TestEqual("The oldest queued edit remains pending", VoxelEdits[0].EditEventIndex, static_cast<uint32>(3));
	TestEqual("Edit mode remains enabled after scheduling a queued edit", Mesher.IsEditEnabled(), true);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesher_InitializeIntervalsWithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately,
	"RDM.RDMMeshersTests.VoxelEventMesher.InitializeIntervals.WithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesher_InitializeIntervalsWithStartEditAndSingleQueuedEditConsumesQueuedEditImmediately::RunTest(
	const FString& Parameters)
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

	TestTrue("All queued edits are consumed into active scheduling when only one follows the start edit",
	         VoxelEdits.IsEmpty());
	TestEqual("Edit mode remains enabled after consuming the queued edit into scheduling", Mesher.IsEditEnabled(),
	          true);
	return true;
}
