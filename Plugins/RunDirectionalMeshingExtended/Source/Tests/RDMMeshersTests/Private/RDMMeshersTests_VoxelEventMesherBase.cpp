#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "Dummies/VoxelEventMesherBaseDummy.h"
#include "VoxelMesher/MeshingUtil/MeshingEvent.h"
#include "Voxel/RLEVoxel.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Updates_When_Smaller,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesWhenSmaller",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Updates_When_Smaller::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Larger,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenLarger",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Larger::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Equal,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenEqual",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Equal::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Next_Is_Zero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoUpdateWhenNextIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Update_When_Next_Is_Zero::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Updates_To_Zero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.UpdatesToZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Updates_To_Zero::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Sets_Indices_To_Zero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.SetsIndicesToZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Sets_Indices_To_Zero::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Does_Nothing_When_Already_Zero,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.DoesNothingWhenAlreadyZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Does_Nothing_When_Already_Zero::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_When_Indices_Match,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWhenIndicesMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_When_Indices_Match::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Advance_When_Indices_Do_Not_Match,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotAdvanceWhenIndicesDoNotMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Advance_When_Indices_Do_Not_Match::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_With_Different_Run_Lengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesWithDifferentRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_With_Different_Run_Lengths::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Multiple_Consecutive_Advances,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.MultipleConsecutiveAdvances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Multiple_Consecutive_Advances::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Zero_Voxel_Index,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithZeroVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Zero_Voxel_Index::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Preserves_Smallest_Next_Index,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.PreservesSmallestNextIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Preserves_Smallest_Next_Index::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_With_Large_Values,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.WithLargeValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_With_Large_Values::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Multiple_Times,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.MultipleTimes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Multiple_Times::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Larger_Grid,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithLargerGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Larger_Grid::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Not_Advanced_When_Index_Too_High,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.NotAdvancedWhenIndexTooHigh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Not_Advanced_When_Index_Too_High::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Voxel_Grid,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleVoxelGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Voxel_Grid::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Sequential_Decreases,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.SequentialDecreases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Sequential_Decreases::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Advance,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterAdvance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Advance::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Very_Large_Run_Lengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithVeryLargeRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Very_Large_Run_Lengths::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_On_Boundary,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesOnBoundary",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_On_Boundary::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Change_On_Sequential_Calls,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.NoChangeOnSequentialCalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_No_Change_On_Sequential_Calls::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_After_Reset,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AfterReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_After_Reset::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Advance_When_Voxel_Index_Beyond_First_Run,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotAdvanceWhenVoxelIndexBeyondFirstRun",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Advance_When_Voxel_Index_Beyond_First_Run::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Updates_Next_Index_Progressively,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.UpdatesNextIndexProgressively",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Updates_Next_Index_Progressively::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Preserves_Event_State,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.PreservesEventState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Preserves_Event_State::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Returns_Correct_Bool_Value,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ReturnsCorrectBoolValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Returns_Correct_Bool_Value::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Min_Function_Behavior,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.MinFunctionBehavior",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Min_Function_Behavior::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Multiple_Grids,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.MultipleGrids",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Multiple_Grids::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Clears_State_Completely,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.ClearsStateCompletely",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Clears_State_Completely::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Identical_Run_Lengths,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithIdenticalRunLengths",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Identical_Run_Lengths::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Allows_Zero_Update,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.AllowsZeroUpdate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Allows_Zero_Update::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Large_Voxel_Index,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithLargeVoxelIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Large_Voxel_Index::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Modify_Event_When_Not_Advanced,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.DoesNotModifyEventWhenNotAdvanced",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Does_Not_Modify_Event_When_Not_Advanced::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Stores_Smallest_Value,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.StoresSmallestValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Stores_Smallest_Value::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_State_Independence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.StateIndependence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_State_Independence::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Handles_Max_Uint32,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.HandlesMaxUint32",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Handles_Max_Uint32::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Maintains_Lowest_Across_Mixed_Sequence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.MaintainsLowestAcrossMixedSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Maintains_Lowest_Across_Mixed_Sequence::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Clears_Indices_After_Try_Update_Sequence,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.ClearsIndicesAfterTryUpdateSequence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_Clears_Indices_After_Try_Update_Sequence::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_At_Second_Run_Boundary_From_Fresh_State,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.AdvancesAtSecondRunBoundaryFromFreshState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Advances_At_Second_Run_Boundary_From_Fresh_State::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Run_Below_Boundary_Does_Not_Advance,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleRunBelowBoundaryDoesNotAdvance",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Run_Below_Boundary_Does_Not_Advance::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Run_Exact_Boundary_Advances,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.WithSingleRunExactBoundaryAdvances",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_With_Single_Run_Exact_Boundary_Advances::RunTest(const FString& Parameters)
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
	FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Advance_And_Try_Update_Restores_Clean_State,
	"RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterAdvanceAndTryUpdateRestoresCleanState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Advance_And_Try_Update_Restores_Clean_State::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Keeps_Lower_Value_After_Large_Candidate,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.KeepsLowerValueAfterLargeCandidate",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Keeps_Lower_Value_After_Large_Candidate::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Stable_On_Repeated_Equal_Candidates,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.TryUpdateNextMeshingEvent.StableOnRepeatedEqualCandidates",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Try_Update_Next_Meshing_Event_Stable_On_Repeated_Equal_Candidates::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Non_Advancing_Attempt_Clears_Indices,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.Reset.AfterNonAdvancingAttemptClearsIndices",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Reset_After_Non_Advancing_Attempt_Clears_Indices::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Third_Run_Boundary_From_Preset_State,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ThirdRunBoundaryFromPresetState",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Third_Run_Boundary_From_Preset_State::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Pre_Boundary_Does_Not_Mutate_Event_Indices,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.PreBoundaryDoesNotMutateEventIndices",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Pre_Boundary_Does_Not_Mutate_Event_Indices::RunTest(const FString& Parameters)
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
 FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Zero_Length_Current_Run_Can_Advance,
 "RDM.RDMMeshersTests.VoxelEventMesherBase.AdvanceMeshingEvent.ZeroLengthCurrentRunCanAdvance",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDM_Meshers_Voxel_Event_Mesher_Base_Advance_Meshing_Event_Zero_Length_Current_Run_Can_Advance::RunTest(const FString& Parameters)
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
