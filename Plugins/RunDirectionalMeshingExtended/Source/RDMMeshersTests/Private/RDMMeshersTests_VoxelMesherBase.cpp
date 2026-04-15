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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 10;
	Mesher.TryUpdateNextMeshingEvent(5);
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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;
	Mesher.TryUpdateNextMeshingEvent(10);
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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;
	Mesher.TryUpdateNextMeshingEvent(5);
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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 0;
	Mesher.TryUpdateNextMeshingEvent(5);
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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 5;
	Mesher.NextVoxelIndex = 10;
	Mesher.Reset();
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
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.Reset();
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

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithDifferentRunLengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWithDifferentRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithDifferentRunLengths::RunTest(const FString& Parameters)
{
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

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)1);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)5);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithMultipleRuns,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWithMultipleRuns",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventAdvancesWithMultipleRuns::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 2;
	Mesher.NextVoxelIndex = 10;

	FRLEVoxel V1;
	V1.RunLenght = 2;
	FRLEVoxel V2;
	V2.RunLenght = 3;
	FRLEVoxel V3;
	V3.RunLenght = 1;
	TSharedPtr<TArray<FRLEVoxel>> Grid = MakeShared<TArray<FRLEVoxel>>();
	Grid->Add(V1);
	Grid->Add(V2);
	Grid->Add(V3);

	FMeshingEvent Event;
	Event.VoxelGridPtr = Grid;
	Event.LastEventIndex = 0;
	Event.VoxelRunIndex = 0;

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)2);
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

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)3);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotUpdateNextWhenLarger,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotUpdateNextWhenLarger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotUpdateNextWhenLarger::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 2;
	Mesher.NextVoxelIndex = 1;

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

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestFalse("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 0);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotUpdateNextAfterAdvanceWhenLarger,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotUpdateNextAfterAdvanceWhenLarger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseAdvanceMeshingEventDoesNotUpdateNextAfterAdvanceWhenLarger::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.VoxelIndex = 3;
	Mesher.NextVoxelIndex = 2;

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

	bool Advanced = Mesher.AdvanceMeshingEvent(Event);
	TestTrue("Advanced", Advanced);
	TestEqual("VoxelRunIndex", Event.VoxelRunIndex, 1);
	TestEqual("LastEventIndex", Event.LastEventIndex, (uint32)3);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesToZero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesToZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersVoxelEventMesherBaseTryUpdateNextMeshingEventUpdatesToZero::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	FVoxelEventMesherBaseDummy Mesher(BaseVoxelDataDummy);
	Mesher.NextVoxelIndex = 5;
	Mesher.TryUpdateNextMeshingEvent(0);
	TestEqual("NextVoxelIndex", Mesher.NextVoxelIndex, (uint32)0);
	return true;
}

