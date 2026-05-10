#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "Dummies/VoxelEventMesherBaseDummy.h"
#include "VoxelMesher/MeshingUtil/MeshingEvent.h"
#include "Voxel/RLEVoxel.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventDoesNotAdvanceWhenEventIndexIsBehindVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotAdvanceWhenEventIndexIsBehindVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventDoesNotAdvanceWhenEventIndexIsBehindVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 2;
	FRLEVoxel V2;
	V2.RunLenght = 1;
	FRLEVoxel V3;
	V3.RunLenght = 4;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 2;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)2);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventZeroLengthRunBehindVoxelIndexStillUpdatesNextIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ZeroLengthRunBehindVoxelIndexStillUpdatesNextIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventZeroLengthRunBehindVoxelIndexStillUpdatesNextIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 4;
	Mesher.NextVoxelIndex = 40;

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 0;
	FRLEVoxel V3;
	V3.RunLenght = 5;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 3;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventAdvancesFromStaggeredMiddleRun,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesFromStaggeredMiddleRun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventAdvancesFromStaggeredMiddleRun::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 1;
	FRLEVoxel V2;
	V2.RunLenght = 2;
	FRLEVoxel V3;
	V3.RunLenght = 4;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 1;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)7);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventAdvanceDoesNotShrinkAlreadyLowerNextVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvanceDoesNotShrinkAlreadyLowerNextVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventAdvanceDoesNotShrinkAlreadyLowerNextVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange: advance WILL happen (EventIndex==VoxelIndex), but the subsequent
	// TryUpdateNextMeshingEvent(9) must NOT overwrite the pre-existing lower value of 5.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 5; // already lower than the next boundary (9)

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 6; // after advance, GetEventIndex = 3+6 = 9
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex stays at pre-existing lower value", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventNextVoxelIndexUpdatedToNextRunBoundaryAfterAdvance,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.NextVoxelIndexUpdatedToNextRunBoundaryAfterAdvance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventNextVoxelIndexUpdatedToNextRunBoundaryAfterAdvance::RunTest(const FString& Parameters)
{
	// Arrange: advance at a staggered run; verify NextVoxelIndex reflects the NEW
	// run's boundary (not the old one) after the advance.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 4;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 1;
	FRLEVoxel V2;
	V2.RunLenght = 3; // GetEventIndex before advance = 1+3 = 4 == VoxelIndex
	FRLEVoxel V3;
	V3.RunLenght = 7; // after advance, new boundary = 4+7 = 11
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 1;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)4);
	TestEqual("NextVoxelIndex set to next run boundary", Mesher.NextVoxelIndex, (uint32)11);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventNoAdvanceWhenEventIndexExceedsVoxelByLargeAmount,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.NoAdvanceWhenEventIndexExceedsVoxelByLargeAmount",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventNoAdvanceWhenEventIndexExceedsVoxelByLargeAmount::RunTest(const FString& Parameters)
{
	// Arrange: EventIndex is vastly larger than VoxelIndex; no advance, but NextVoxelIndex
	// must still be updated to EventIndex via TryUpdateNextMeshingEvent.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 1;
	Mesher.NextVoxelIndex = 5000;

	FRLEVoxel V1;
	V1.RunLenght = 1000; // GetEventIndex = 0+1000 = 1000 >> VoxelIndex=1
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex unchanged", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex unchanged", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex updated to EventIndex", Mesher.NextVoxelIndex, (uint32)1000);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventAdvancesOnFourthRunBoundaryFromPresetState,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesOnFourthRunBoundaryFromPresetState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventAdvancesOnFourthRunBoundaryFromPresetState::RunTest(const FString& Parameters)
{
	// Arrange: 5-run grid, event pre-positioned at run index 3; advance should
	// move to run index 4 and update NextVoxelIndex to that run's far boundary.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 11; // 2+3+2+4 = 11
	Mesher.NextVoxelIndex = 200;

	FRLEVoxel V1; V1.RunLenght = 2;
	FRLEVoxel V2; V2.RunLenght = 3;
	FRLEVoxel V3; V3.RunLenght = 2;
	FRLEVoxel V4; V4.RunLenght = 4; // boundary at 2+3+2+4=11 == VoxelIndex
	FRLEVoxel V5; V5.RunLenght = 5; // after advance, boundary = 11+5=16
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);
	Grid->Add(V4);
	Grid->Add(V5);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 7; // 2+3+2
	Event.VoxelRunIndex = 3;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 4);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)11);
	TestEqual("NextVoxelIndex set to fifth run boundary", Mesher.NextVoxelIndex, (uint32)16);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventOffByOneEventIndexDoesNotAdvanceButUpdatesNextVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.OffByOneEventIndexDoesNotAdvanceButUpdatesNextVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventOffByOneEventIndexDoesNotAdvanceButUpdatesNextVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange: EventIndex is exactly VoxelIndex+1 (off by one); no advance, but
	// NextVoxelIndex must still shrink from 1000 to 11.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 10;
	Mesher.NextVoxelIndex = 1000;

	FRLEVoxel V1; V1.RunLenght = 5;
	FRLEVoxel V2; V2.RunLenght = 6; // GetEventIndex = 5+6 = 11 (VoxelIndex is 10)
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 5;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex unchanged", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex unchanged", Event.LastEventIndex, (uint32)5);
	TestEqual("NextVoxelIndex updated to EventIndex", Mesher.NextVoxelIndex, (uint32)11);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventAdvancesAtZeroLengthFirstRunWithVoxelIndexZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesAtZeroLengthFirstRunWithVoxelIndexZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventAdvancesAtZeroLengthFirstRunWithVoxelIndexZero::RunTest(const FString& Parameters)
{
	// Arrange: Run 0 has length 0 and VoxelIndex is 0, so GetEventIndex()==0==VoxelIndex.
	// This advances from run 0 directly to run 1 (distinct from ZeroLengthCurrentRunCanAdvance
	// which starts at run index 1 already).
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 0;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 0; // GetEventIndex = 0+0 = 0 == VoxelIndex
	FRLEVoxel V2;
	V2.RunLenght = 8; // after advance, boundary = 0+8 = 8
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)8);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventNextVoxelIndexUnchangedWhenEventIndexEqualsExistingNextVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.NextVoxelIndexUnchangedWhenEventIndexEqualsExistingNextVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventNextVoxelIndexUnchangedWhenEventIndexEqualsExistingNextVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange: No advance (EventIndex != VoxelIndex), but EventIndex == NextVoxelIndex.
	// TryUpdateNextMeshingEvent(12) with NextVoxelIndex already 12 must leave it unchanged.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 7;
	Mesher.NextVoxelIndex = 12;

	FRLEVoxel V1;
	V1.RunLenght = 12; // GetEventIndex = 0+12 = 12 != VoxelIndex(7)
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex unchanged", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex unchanged", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex unchanged", Mesher.NextVoxelIndex, (uint32)12);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_ResetZeroesVeryLargeVoxelAndNextIndices,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.ZeroesVeryLargeVoxelAndNextIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_ResetZeroesVeryLargeVoxelAndNextIndices::RunTest(const FString& Parameters)
{
	// Arrange: both indices set to large arbitrary values well below MAX_UINT32.
	// Reset must unconditionally zero both regardless of magnitude.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 999999;
	Mesher.NextVoxelIndex = 888888;

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex zeroed", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex zeroed", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_AdvanceMeshingEventAdvancesFromSecondOfTwoShortConsecutiveRuns,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesFromSecondOfTwoShortConsecutiveRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_AdvanceMeshingEventAdvancesFromSecondOfTwoShortConsecutiveRuns::RunTest(const FString& Parameters)
{
	// Arrange: V1(len=3) + V2(len=2) = boundary at 5. Event is at run 1 (the second
	// short run). VoxelIndex=5 matches that boundary, so advance to run 2.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1; V1.RunLenght = 3;
	FRLEVoxel V2; V2.RunLenght = 2; // boundary = 3+2 = 5 == VoxelIndex
	FRLEVoxel V3; V3.RunLenght = 4; // after advance, boundary = 5+4 = 9
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 3;
	Event.VoxelRunIndex = 1;

	// Act
	const bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)9);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FVoxelEventMesherBase_TryUpdateNextMeshingEventUpdatesToZeroFromMaxUint32,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesToZeroFromMaxUint32",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FVoxelEventMesherBase_TryUpdateNextMeshingEventUpdatesToZeroFromMaxUint32::RunTest(const FString& Parameters)
{
	// Arrange: NextVoxelIndex starts at the maximum possible uint32 value.
	// Calling TryUpdateNextMeshingEvent(0) must bring it all the way down to 0.
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = TNumericLimits<uint32>::Max();

	// Act
	Mesher.TryUpdateNextMeshingEvent(0);

	// Assert
	TestEqual("NextVoxelIndex updated to 0", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

