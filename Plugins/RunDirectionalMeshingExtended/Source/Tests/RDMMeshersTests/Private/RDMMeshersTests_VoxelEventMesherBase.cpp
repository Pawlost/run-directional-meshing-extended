#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "Dummies/VoxelEventMesherBaseDummy.h"
#include "VoxelMesher/MeshingUtil/MeshingEvent.h"
#include "Voxel/RLEVoxel.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesWhenSmaller,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesWhenSmaller",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesWhenSmaller::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 10;

	// Act
	Mesher.TryUpdateNextMeshingEvent(5);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenLarger,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenLarger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenLarger::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;

	// Act
	Mesher.TryUpdateNextMeshingEvent(10);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenEqual,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenEqual",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenEqual::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;

	// Act
	Mesher.TryUpdateNextMeshingEvent(5);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenNextIsZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenNextIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoUpdateWhenNextIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 0;

	// Act
	Mesher.TryUpdateNextMeshingEvent(5);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesToZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesToZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesToZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;

	// Act
	Mesher.TryUpdateNextMeshingEvent(0);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetSetsIndicesToZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.SetsIndicesToZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetSetsIndicesToZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 10;

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetDoesNothingWhenAlreadyZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.DoesNothingWhenAlreadyZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetDoesNothingWhenAlreadyZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWhenIndicesMatch,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWhenIndicesMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWhenIndicesMatch::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 10;

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 2;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotAdvanceWhenIndicesDoNotMatch,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotAdvanceWhenIndicesDoNotMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotAdvanceWhenIndicesDoNotMatch::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 2;
	Mesher.NextVoxelIndex = 10;

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 2;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithDifferentRunLengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWithDifferentRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithDifferentRunLengths::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 1;
	Mesher.NextVoxelIndex = 10;

	FRLEVoxel V1;
	V1.RunLenght = 1;
	FRLEVoxel V2;
	V2.RunLenght = 4;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)1);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventMultipleConsecutiveAdvances,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.MultipleConsecutiveAdvances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventMultipleConsecutiveAdvances::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 0;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 1;
	FRLEVoxel V2;
	V2.RunLenght = 2;
	FRLEVoxel V3;
	V3.RunLenght = 3;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool FirstAdvanced = Mesher.AdvanceMeshingEvent(Event);
	Mesher.VoxelIndex = 1;
	bool SecondAdvanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("FirstAdvanced", FirstAdvanced);
	TestTrue("SecondAdvanced", SecondAdvanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithZeroVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithZeroVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithZeroVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 0;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	FRLEVoxel V2;
	V2.RunLenght = 3;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventPreservesSmallestNextIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.PreservesSmallestNextIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventPreservesSmallestNextIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 4;
	Mesher.NextVoxelIndex = 2;

	FRLEVoxel V1;
	V1.RunLenght = 4;
	FRLEVoxel V2;
	V2.RunLenght = 6;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("NextVoxelIndex preserved", Mesher.NextVoxelIndex, (uint32)2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventWithLargeValues,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.WithLargeValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventWithLargeValues::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 1000000;

	// Act
	Mesher.TryUpdateNextMeshingEvent(500000);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)500000);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetMultipleTimes,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.MultipleTimes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetMultipleTimes::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 100;
	Mesher.NextVoxelIndex = 200;

	// Act
	Mesher.Reset();
	Mesher.VoxelIndex = 50;
	Mesher.NextVoxelIndex = 75;
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithLargerGrid,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithLargerGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithLargerGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 2;
	FRLEVoxel V2;
	V2.RunLenght = 3;
	FRLEVoxel V3;
	V3.RunLenght = 5;
	FRLEVoxel V4;
	V4.RunLenght = 4;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);
	Grid->Add(V4);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventNotAdvancedWhenIndexTooHigh,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.NotAdvancedWhenIndexTooHigh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventNotAdvancedWhenIndexTooHigh::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 100;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 10;
	FRLEVoxel V2;
	V2.RunLenght = 20;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleVoxelGrid,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleVoxelGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleVoxelGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 7;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 7;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)7);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)7);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventSequentialDecreases,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.SequentialDecreases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventSequentialDecreases::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 100;

	// Act
	Mesher.TryUpdateNextMeshingEvent(50);
	Mesher.TryUpdateNextMeshingEvent(25);
	Mesher.TryUpdateNextMeshingEvent(10);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetAfterAdvance,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterAdvance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetAfterAdvance::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 2;
	Mesher.NextVoxelIndex = 20;

	FRLEVoxel V1;
	V1.RunLenght = 2;
	FRLEVoxel V2;
	V2.RunLenght = 5;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	Mesher.Reset();

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithVeryLargeRunLengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithVeryLargeRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithVeryLargeRunLengths::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 50000;
	Mesher.NextVoxelIndex = 100000;

	FRLEVoxel V1;
	V1.RunLenght = 50000;
	FRLEVoxel V2;
	V2.RunLenght = 75000;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)50000);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)50000);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesOnBoundary,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesOnBoundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesOnBoundary::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 10;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	FRLEVoxel V2;
	V2.RunLenght = 10;
	FRLEVoxel V3;
	V3.RunLenght = 15;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)10);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)15);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoChangeOnSequentialCalls,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoChangeOnSequentialCalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventNoChangeOnSequentialCalls::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 20;

	// Act
	Mesher.TryUpdateNextMeshingEvent(25);
	Mesher.TryUpdateNextMeshingEvent(30);
	Mesher.TryUpdateNextMeshingEvent(35);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)20);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAfterReset,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AfterReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAfterReset::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 0;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	Mesher.Reset();
	Mesher.VoxelIndex = 0;
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelIndex after advance", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotAdvanceWhenVoxelIndexBeyondFirstRun,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotAdvanceWhenVoxelIndexBeyondFirstRun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotAdvanceWhenVoxelIndexBeyondFirstRun::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 10;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	FRLEVoxel V2;
	V2.RunLenght = 8;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex unchanged", Event.VoxelRunIndex, 0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventUpdatesNextIndexProgressively,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.UpdatesNextIndexProgressively",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventUpdatesNextIndexProgressively::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 0;
	Mesher.NextVoxelIndex = 1000;

	FRLEVoxel V1;
	V1.RunLenght = 0;
	FRLEVoxel V2;
	V2.RunLenght = 3;
	FRLEVoxel V3;
	V3.RunLenght = 7;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("NextVoxelIndex updated to smallest", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetPreservesEventState,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.PreservesEventState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetPreservesEventState::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 15;
	Mesher.NextVoxelIndex = 30;

	FRLEVoxel V1;
	V1.RunLenght = 10;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 5;
	Event.VoxelRunIndex = 1;

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	TestEqual("Event LastEventIndex unchanged", Event.LastEventIndex, (uint32)5);
	TestEqual("Event VoxelRunIndex unchanged", Event.VoxelRunIndex, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventReturnsCorrectBoolValue,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ReturnsCorrectBoolValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventReturnsCorrectBoolValue::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 8;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 8;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Result = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Result is true when advanced", Result);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventMinFunctionBehavior,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.MinFunctionBehavior",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventMinFunctionBehavior::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 42;

	// Act
	Mesher.TryUpdateNextMeshingEvent(42);

	// Assert
	TestEqual("NextVoxelIndex remains same when equal", Mesher.NextVoxelIndex, (uint32)42);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventMultipleGrids,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.MultipleGrids",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventMultipleGrids::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 15;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	FRLEVoxel V2;
	V2.RunLenght = 10;
	FRLEVoxel V3;
	V3.RunLenght = 15;
	FRLEVoxel V4;
	V4.RunLenght = 20;
	FRLEVoxel V5;
	V5.RunLenght = 25;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);
	Grid->Add(V4);
	Grid->Add(V5);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)20);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetClearsStateCompletely,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.ClearsStateCompletely",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetClearsStateCompletely::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 9999;
	Mesher.NextVoxelIndex = 8888;

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithIdenticalRunLengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithIdenticalRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithIdenticalRunLengths::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 6;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 6;
	FRLEVoxel V2;
	V2.RunLenght = 6;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)6);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventAllowsZeroUpdate,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.AllowsZeroUpdate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventAllowsZeroUpdate::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 100;

	// Act
	Mesher.TryUpdateNextMeshingEvent(1);
	Mesher.TryUpdateNextMeshingEvent(0);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithLargeVoxelIndex,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithLargeVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithLargeVoxelIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 999999;
	Mesher.NextVoxelIndex = 2000000;

	FRLEVoxel V1;
	V1.RunLenght = 999999;
	FRLEVoxel V2;
	V2.RunLenght = 1000000;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)1000000);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotModifyEventWhenNotAdvanced,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotModifyEventWhenNotAdvanced",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotModifyEventWhenNotAdvanced::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 7;
	Mesher.NextVoxelIndex = 50;

	FRLEVoxel V1;
	V1.RunLenght = 12;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 99;
	Event.VoxelRunIndex = 88;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("LastEventIndex unchanged", Event.LastEventIndex, (uint32)99);
	TestEqual("VoxelRunIndex unchanged", Event.VoxelRunIndex, 88);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventStoresSmallestValue,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.StoresSmallestValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventStoresSmallestValue::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 15;

	// Act
	Mesher.TryUpdateNextMeshingEvent(20);
	Mesher.TryUpdateNextMeshingEvent(10);
	Mesher.TryUpdateNextMeshingEvent(12);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventStateIndependence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.StateIndependence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventStateIndependence::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 5;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event1;
	Event1.VoxelGridPtr = Grid;
	Event1.LastEventIndex = 0;
	Event1.VoxelRunIndex = 0;

	FMeshingEvent Event2;
	Event2.VoxelGridPtr = Grid;
	Event2.LastEventIndex = 0;
	Event2.VoxelRunIndex = 0;

	// Act
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 10;
	bool Advanced1 = Mesher.AdvanceMeshingEvent(Event1);

	Mesher.VoxelIndex = 2;
	Mesher.NextVoxelIndex = 10;
	bool Advanced2 = Mesher.AdvanceMeshingEvent(Event2);

	// Assert
	TestTrue("First advance succeeded", Advanced1);
	TestFalse("Second advance failed", Advanced2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventHandlesMaxUint32,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.HandlesMaxUint32",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventHandlesMaxUint32::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = (uint32)-1;

	// Act
	Mesher.TryUpdateNextMeshingEvent((uint32)-2);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)-2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventMaintainsLowestAcrossMixedSequence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.MaintainsLowestAcrossMixedSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventMaintainsLowestAcrossMixedSequence::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 100;

	// Act
	Mesher.TryUpdateNextMeshingEvent(60);
	Mesher.TryUpdateNextMeshingEvent(80);
	Mesher.TryUpdateNextMeshingEvent(20);
	Mesher.TryUpdateNextMeshingEvent(25);

	// Assert
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)20);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetClearsIndicesAfterTryUpdateSequence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.ClearsIndicesAfterTryUpdateSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetClearsIndicesAfterTryUpdateSequence::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 44;
	Mesher.NextVoxelIndex = 90;
	Mesher.TryUpdateNextMeshingEvent(12);

	// Act
	Mesher.Reset();

	// Assert
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesAtSecondRunBoundaryFromFreshState,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesAtSecondRunBoundaryFromFreshState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesAtSecondRunBoundaryFromFreshState::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 500;

	FRLEVoxel V1;
	V1.RunLenght = 2;
	FRLEVoxel V2;
	V2.RunLenght = 3;
	FRLEVoxel V3;
	V3.RunLenght = 6;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleRunBelowBoundaryDoesNotAdvance,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleRunBelowBoundaryDoesNotAdvance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleRunBelowBoundaryDoesNotAdvance::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 4;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleRunExactBoundaryAdvances,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleRunExactBoundaryAdvances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventWithSingleRunExactBoundaryAdvances::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 100;

	FRLEVoxel V1;
	V1.RunLenght = 5;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseResetAfterAdvanceAndTryUpdateRestoresCleanState,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterAdvanceAndTryUpdateRestoresCleanState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetAfterAdvanceAndTryUpdateRestoresCleanState::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 200;

	FRLEVoxel V1;
	V1.RunLenght = 3;
	FRLEVoxel V2;
	V2.RunLenght = 7;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	// Act
	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	Mesher.TryUpdateNextMeshingEvent(1);
	Mesher.Reset();

	// Assert
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventKeepsLowerValueAfterLargeCandidate,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.KeepsLowerValueAfterLargeCandidate",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventKeepsLowerValueAfterLargeCandidate::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.NextVoxelIndex = 50;

 // Act
 Mesher.TryUpdateNextMeshingEvent(10);
 Mesher.TryUpdateNextMeshingEvent(1000);

 // Assert
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)10);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventStableOnRepeatedEqualCandidates,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.StableOnRepeatedEqualCandidates",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventStableOnRepeatedEqualCandidates::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.NextVoxelIndex = 33;

 // Act
 Mesher.TryUpdateNextMeshingEvent(33);
 Mesher.TryUpdateNextMeshingEvent(33);
 Mesher.TryUpdateNextMeshingEvent(33);

 // Assert
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)33);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseResetAfterNonAdvancingAttemptClearsIndices,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterNonAdvancingAttemptClearsIndices",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseResetAfterNonAdvancingAttemptClearsIndices::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.VoxelIndex = 1;
 Mesher.NextVoxelIndex = 100;

 FRLEVoxel V1;
 V1.RunLenght = 5;
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);

 FMeshingEvent Event;
 Event.VoxelGridPtr = Grid;
 Event.LastEventIndex = 0;
 Event.VoxelRunIndex = 0;

 // Act
 bool Advanced = Mesher.AdvanceMeshingEvent(Event);
 Mesher.Reset();

 // Assert
 TestFalse("Advanced", Advanced);
 TestEqual("VoxelIndex", Mesher.VoxelIndex, (uint32)0);
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventThirdRunBoundaryFromPresetState,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ThirdRunBoundaryFromPresetState",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventThirdRunBoundaryFromPresetState::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.VoxelIndex = 9;
 Mesher.NextVoxelIndex = 100;

 FRLEVoxel V1;
 V1.RunLenght = 2;
 FRLEVoxel V2;
 V2.RunLenght = 3;
 FRLEVoxel V3;
 V3.RunLenght = 4;
 FRLEVoxel V4;
 V4.RunLenght = 6;
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);
 Grid->Add(V3);
 Grid->Add(V4);

 FMeshingEvent Event;
 Event.VoxelGridPtr = Grid;
 Event.LastEventIndex = 5;
 Event.VoxelRunIndex = 2;

 // Act
 bool Advanced = Mesher.AdvanceMeshingEvent(Event);

 // Assert
 TestTrue("Advanced", Advanced);
 TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 3);
 TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)9);
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)15);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventPreBoundaryDoesNotMutateEventIndices,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.PreBoundaryDoesNotMutateEventIndices",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventPreBoundaryDoesNotMutateEventIndices::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.VoxelIndex = 8;
 Mesher.NextVoxelIndex = 100;

 FRLEVoxel V1;
 V1.RunLenght = 2;
 FRLEVoxel V2;
 V2.RunLenght = 3;
 FRLEVoxel V3;
 V3.RunLenght = 4;
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);
 Grid->Add(V3);

 FMeshingEvent Event;
 Event.VoxelGridPtr = Grid;
 Event.LastEventIndex = 5;
 Event.VoxelRunIndex = 2;

 // Act
 bool Advanced = Mesher.AdvanceMeshingEvent(Event);

 // Assert
 TestFalse("Advanced", Advanced);
 TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
 TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)9);
 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventZeroLengthCurrentRunCanAdvance,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ZeroLengthCurrentRunCanAdvance",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventZeroLengthCurrentRunCanAdvance::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
 Mesher.VoxelIndex = 5;
 Mesher.NextVoxelIndex = 200;

 FRLEVoxel V1;
 V1.RunLenght = 5;
 FRLEVoxel V2;
 V2.RunLenght = 0;
 FRLEVoxel V3;
 V3.RunLenght = 4;
 TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
 Grid->Add(V1);
 Grid->Add(V2);
 Grid->Add(V3);

 FMeshingEvent Event;
 Event.VoxelGridPtr = Grid;
 Event.LastEventIndex = 5;
 Event.VoxelRunIndex = 1;

 // Act
 bool Advanced = Mesher.AdvanceMeshingEvent(Event);

 // Assert
 TestTrue("Advanced", Advanced);
 TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 2);
 TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)5);
 TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)9);
 return true;
}
