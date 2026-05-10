#include "BaseVoxelData.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "Misc/AutomationTest.h"


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_RecalculateVoxelData_UpdatesCachedValues,
	"RDM.RDMMeshersTests.BaseVoxelData.RecalculateVoxelData.UpdatesCachedValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_RecalculateVoxelData_UpdatesCachedValues::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->VoxelSize = 1.5;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;
	BaseVoxelDataDummy->VoxelSize = 0.25;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Voxel line should match the updated dimension.",
		BaseVoxelDataDummy->GetVoxelLine(), 16u);
	TestEqual("Voxel plane should be recalculated after dimension update.",
		BaseVoxelDataDummy->GetVoxelPlane(), 256u);
	TestEqual("Chunk size should be recalculated after dimension update.",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(), 4096u);
	TestEqual("Chunk spacing should be recalculated after dimension and voxel size update.",
		BaseVoxelDataDummy->GetChunkSpacing(), 4.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_YStepIncreasesByOne,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.YStepIncreasesByOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_YStepIncreasesByOne::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 BaseIndex = BaseVoxelDataDummy->CalculateVoxelIndex(4, 6, 7);
	const uint32 NextYIndex = BaseVoxelDataDummy->CalculateVoxelIndex(4, 7, 7);

	// Assert
	TestEqual("Increasing Y by one should increment index by one.",
		NextYIndex - BaseIndex, 1u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZStepIncreasesByVoxelLine,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.ZStepIncreasesByVoxelLine",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZStepIncreasesByVoxelLine::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 BaseIndex = BaseVoxelDataDummy->CalculateVoxelIndex(4, 6, 7);
	const uint32 NextZIndex = BaseVoxelDataDummy->CalculateVoxelIndex(4, 6, 8);

	// Assert
	TestEqual("Increasing Z by one should increment index by voxel line size.",
		NextZIndex - BaseIndex, BaseVoxelDataDummy->GetVoxelLine());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_XStepIncreasesByVoxelPlane,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.XStepIncreasesByVoxelPlane",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_XStepIncreasesByVoxelPlane::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 BaseIndex = BaseVoxelDataDummy->CalculateVoxelIndex(4, 6, 7);
	const uint32 NextXIndex = BaseVoxelDataDummy->CalculateVoxelIndex(5, 6, 7);

	// Assert
	TestEqual("Increasing X by one should increment index by voxel plane size.",
		NextXIndex - BaseIndex, BaseVoxelDataDummy->GetVoxelPlane());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ZeroVoxelCount_ProducesZeroChunkMetrics,
	"RDM.RDMMeshersTests.BaseVoxelData.ZeroVoxelCount.ProducesZeroChunkMetrics",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ZeroVoxelCount_ProducesZeroChunkMetrics::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 0;
	BaseVoxelDataDummy->VoxelSize = 3.0;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Voxel line should be zero when chunk dimension is zero.",
		BaseVoxelDataDummy->GetVoxelLine(), 0u);
	TestEqual("Voxel plane should be zero when chunk dimension is zero.",
		BaseVoxelDataDummy->GetVoxelPlane(), 0u);
	TestEqual("Chunk size should be zero when chunk dimension is zero.",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(), 0u);
	TestEqual("Chunk spacing should be zero when chunk dimension is zero.",
		BaseVoxelDataDummy->GetChunkSpacing(), 0.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_HighestElevation_IgnoresLocationAndMatchesChunkSpacing,
	"RDM.RDMMeshersTests.BaseVoxelData.HighestElevation.IgnoresLocationAndMatchesChunkSpacing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_HighestElevation_IgnoresLocationAndMatchesChunkSpacing::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->VoxelSize = 0.75;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const double ElevationAtOrigin = BaseVoxelDataDummy->GetHighestElevationAtLocation(FVector::ZeroVector);
	const double ElevationAtOffset = BaseVoxelDataDummy->GetHighestElevationAtLocation(FVector(100.0, -250.0, 33.0));

	// Assert
	TestEqual("Highest elevation should equal chunk spacing.",
		ElevationAtOrigin, BaseVoxelDataDummy->GetChunkSpacing());
	TestEqual("Highest elevation should currently be location-independent.",
		ElevationAtOffset, ElevationAtOrigin);

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_DimensionOne_ProducesSingleVoxelChunk,
	"RDM.RDMMeshersTests.BaseVoxelData.DimensionOne.ProducesSingleVoxelChunk",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_DimensionOne_ProducesSingleVoxelChunk::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 1;
	BaseVoxelDataDummy->VoxelSize = 2.5;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);

	// Assert
	TestEqual("Single-voxel chunk should report one voxel per dimension.",
		BaseVoxelDataDummy->GetVoxelLine(), 1u);
	TestEqual("Single-voxel chunk should have one voxel in a plane.",
		BaseVoxelDataDummy->GetVoxelPlane(), 1u);
	TestEqual("Single-voxel chunk should have one voxel total.",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(), 1u);
	TestEqual("Single-voxel chunk spacing should equal voxel size.",
		BaseVoxelDataDummy->GetChunkSpacing(), 2.5);
	TestEqual("Only valid voxel in a single-voxel chunk should map to index zero.",
		Index, 0u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ZeroVoxelSize_ChunkSpacingIsZero,
	"RDM.RDMMeshersTests.BaseVoxelData.ZeroVoxelSize.ChunkSpacingIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ZeroVoxelSize_ChunkSpacingIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->VoxelSize = 0.0;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(1, 2, 3);

	// Assert
	TestEqual("Chunk spacing should be zero when voxel size is zero.",
		BaseVoxelDataDummy->GetChunkSpacing(), 0.0);
	TestEqual("Voxel indexing should still follow chunk dimensions when voxel size is zero.",
		Index, 1122u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_CornerCoordinatesProduceUniqueIndices,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.CornerCoordinatesProduceUniqueIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_CornerCoordinatesProduceUniqueIndices::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 4;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	TSet<uint32> CornerIndices;
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 3));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(0, 3, 0));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(0, 3, 3));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(3, 0, 0));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(3, 0, 3));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(3, 3, 0));
	CornerIndices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(3, 3, 3));

	// Assert
	TestEqual("All eight chunk corners should map to unique indices.",
		CornerIndices.Num(), 8);
	TestTrue("Highest corner index should still be inside the chunk.",
		BaseVoxelDataDummy->CalculateVoxelIndex(3, 3, 3) < BaseVoxelDataDummy->GetMaxVoxelsInChunk());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_LastCoordinateOfLargeChunkMatchesMaxMinusOne,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.LastCoordinateOfLargeChunkMatchesMaxMinusOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_LastCoordinateOfLargeChunkMatchesMaxMinusOne::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 128;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 LastIndex = BaseVoxelDataDummy->CalculateVoxelIndex(127, 127, 127);

	// Assert
	TestEqual("Last coordinate in a large chunk should map to max voxel count minus one.",
		LastIndex, BaseVoxelDataDummy->GetMaxVoxelsInChunk() - 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_NonPowerOfTwoDimension_MetricsAreConsistent,
	"RDM.RDMMeshersTests.BaseVoxelData.NonPowerOfTwoDimension.MetricsAreConsistent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_NonPowerOfTwoDimension_MetricsAreConsistent::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 10;
	BaseVoxelDataDummy->VoxelSize = 2.0;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Voxel line should equal the non-power-of-two dimension.",
		BaseVoxelDataDummy->GetVoxelLine(), 10u);
	TestEqual("Voxel plane should be dimension squared.",
		BaseVoxelDataDummy->GetVoxelPlane(), 100u);
	TestEqual("Chunk size should be dimension cubed.",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(), 1000u);
	TestEqual("Chunk spacing should be dimension times voxel size.",
		BaseVoxelDataDummy->GetChunkSpacing(), 20.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelPlane_AlwaysEqualsVoxelLineSquared,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlane.AlwaysEqualsVoxelLineSquared",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelPlane_AlwaysEqualsVoxelLineSquared::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Line = BaseVoxelDataDummy->GetVoxelLine();

	// Assert
	TestEqual("Voxel plane should always be voxel line multiplied by itself.",
		BaseVoxelDataDummy->GetVoxelPlane(), Line * Line);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_MaxVoxelsInChunk_AlwaysEqualsVoxelPlaneTimesVoxelLine,
	"RDM.RDMMeshersTests.BaseVoxelData.MaxVoxelsInChunk.AlwaysEqualsVoxelPlaneTimesVoxelLine",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_MaxVoxelsInChunk_AlwaysEqualsVoxelPlaneTimesVoxelLine::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 20;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Max voxels in chunk should always be voxel plane times voxel line.",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(),
		BaseVoxelDataDummy->GetVoxelPlane() * BaseVoxelDataDummy->GetVoxelLine());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelSize_ReflectsUpdatedValueAfterRecalculation,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelSize.ReflectsUpdatedValueAfterRecalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelSize_ReflectsUpdatedValueAfterRecalculation::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelSize = 1.0;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	BaseVoxelDataDummy->VoxelSize = 5.0;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("GetVoxelSize should return the most recently set voxel size.",
		BaseVoxelDataDummy->GetVoxelSize(), 5.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing_ScalesLinearlyWithVoxelSize,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing.ScalesLinearlyWithVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing_ScalesLinearlyWithVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->VoxelSize = 3.0;
	BaseVoxelDataDummy->CalculateVoxelData();
	const double BaseSpacing = BaseVoxelDataDummy->GetChunkSpacing();

	// Act
	BaseVoxelDataDummy->VoxelSize = 6.0;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Doubling voxel size should double the chunk spacing.",
		BaseVoxelDataDummy->GetChunkSpacing(), BaseSpacing * 2.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_NegativeXCoordinateIsOutOfBounds,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.NegativeXCoordinateIsOutOfBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_NegativeXCoordinateIsOutOfBounds::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(-1, 0, 0);

	// Assert
	TestTrue("A negative X coordinate should produce an index outside the chunk bounds.",
		Index > BaseVoxelDataDummy->GetMaxVoxelsInChunk());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_NegativeZCoordinateIsOutOfBounds,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.NegativeZCoordinateIsOutOfBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_NegativeZCoordinateIsOutOfBounds::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, -1);

	// Assert
	TestTrue("A negative Z coordinate should produce an index outside the chunk bounds.",
		Index > BaseVoxelDataDummy->GetMaxVoxelsInChunk());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_SameCoordinatesAlwaysProduceSameIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.SameCoordinatesAlwaysProduceSameIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_SameCoordinatesAlwaysProduceSameIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 FirstCall = BaseVoxelDataDummy->CalculateVoxelIndex(5, 10, 15);
	const uint32 SecondCall = BaseVoxelDataDummy->CalculateVoxelIndex(5, 10, 15);

	// Assert
	TestEqual("Calling CalculateVoxelIndex with the same coordinates twice should return the same index.",
		FirstCall, SecondCall);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_HighestElevation_UpdatesAfterRecalculation,
	"RDM.RDMMeshersTests.BaseVoxelData.HighestElevation.UpdatesAfterRecalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_HighestElevation_UpdatesAfterRecalculation::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->VoxelSize = 1.0;
	BaseVoxelDataDummy->CalculateVoxelData();
	const double ElevationBefore = BaseVoxelDataDummy->GetHighestElevationAtLocation(FVector::ZeroVector);

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;
	BaseVoxelDataDummy->VoxelSize = 2.0;
	BaseVoxelDataDummy->CalculateVoxelData();
	const double ElevationAfter = BaseVoxelDataDummy->GetHighestElevationAtLocation(FVector::ZeroVector);

	// Assert
	TestNotEqual("Highest elevation should change after chunk dimensions and voxel size are updated.",
		ElevationAfter, ElevationBefore);
	TestEqual("Updated highest elevation should match new chunk spacing.",
		ElevationAfter, BaseVoxelDataDummy->GetChunkSpacing());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_MidpointCoordinateIsInsideChunkBounds,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.MidpointCoordinateIsInsideChunkBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_MidpointCoordinateIsInsideChunkBounds::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 MidIndex = BaseVoxelDataDummy->CalculateVoxelIndex(
		NumberOfVoxels / 2, NumberOfVoxels / 2, NumberOfVoxels / 2);

	// Assert
	TestTrue("A midpoint voxel coordinate should produce an index inside the chunk.",
		MidIndex < BaseVoxelDataDummy->GetMaxVoxelsInChunk());
	TestTrue("A midpoint voxel index should be greater than zero.",
		MidIndex > 0u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_TwoAdjacentVoxelsAlongYHaveConsecutiveIndices,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.TwoAdjacentVoxelsAlongYHaveConsecutiveIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_TwoAdjacentVoxelsAlongYHaveConsecutiveIndices::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 A = BaseVoxelDataDummy->CalculateVoxelIndex(2, 0, 3);
	const uint32 B = BaseVoxelDataDummy->CalculateVoxelIndex(2, 1, 3);
	const uint32 C = BaseVoxelDataDummy->CalculateVoxelIndex(2, 2, 3);

	// Assert
	TestEqual("Consecutive Y steps should produce consecutive indices.",
		B - A, C - B);
	TestEqual("Step size along Y should be exactly one.",
		B - A, 1u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_FIntVectorOverloadMatchesDirectCallForNegativeInput,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.FIntVectorOverloadMatchesDirectCallForNegativeInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_FIntVectorOverloadMatchesDirectCallForNegativeInput::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 DirectIndex = BaseVoxelDataDummy->CalculateVoxelIndex(-1, -2, -3);
	const uint32 VectorIndex = BaseVoxelDataDummy->CalculateVoxelIndex(FIntVector(-1, -2, -3));

	// Assert
	TestEqual("FIntVector overload should produce the same result as direct call for negative coordinates.",
		VectorIndex, DirectIndex);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing_IsProductOfVoxelLineAndVoxelSize,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing.IsProductOfVoxelLineAndVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing_IsProductOfVoxelLineAndVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 24;
	BaseVoxelDataDummy->VoxelSize = 1.25;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Chunk spacing should equal voxel line times voxel size.",
		BaseVoxelDataDummy->GetChunkSpacing(),
		static_cast<double>(BaseVoxelDataDummy->GetVoxelLine()) * BaseVoxelDataDummy->GetVoxelSize());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexesAreMonotonicallyIncreasingAlongX,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.IndexesAreMonotonicallyIncreasingAlongX",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexesAreMonotonicallyIncreasingAlongX::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 X0 = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	const uint32 X1 = BaseVoxelDataDummy->CalculateVoxelIndex(1, 0, 0);
	const uint32 X2 = BaseVoxelDataDummy->CalculateVoxelIndex(2, 0, 0);
	const uint32 X3 = BaseVoxelDataDummy->CalculateVoxelIndex(3, 0, 0);

	// Assert
	TestTrue("Index should increase when X increases from 0 to 1.", X1 > X0);
	TestTrue("Index should increase when X increases from 1 to 2.", X2 > X1);
	TestTrue("Index should increase when X increases from 2 to 3.", X3 > X2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexesAreMonotonicallyIncreasingAlongZ,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.IndexesAreMonotonicallyIncreasingAlongZ",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexesAreMonotonicallyIncreasingAlongZ::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 Z0 = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	const uint32 Z1 = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 1);
	const uint32 Z2 = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 2);
	const uint32 Z3 = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 3);

	// Assert
	TestTrue("Index should increase when Z increases from 0 to 1.", Z1 > Z0);
	TestTrue("Index should increase when Z increases from 1 to 2.", Z2 > Z1);
	TestTrue("Index should increase when Z increases from 2 to 3.", Z3 > Z2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_DistinctCoordinatesProduceDistinctIndices,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.DistinctCoordinatesProduceDistinctIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_DistinctCoordinatesProduceDistinctIndices::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	TSet<uint32> Indices;
	for (int32 X = 0; X < 8; ++X)
	{
		for (int32 Z = 0; Z < 8; ++Z)
		{
			Indices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(X, 0, Z));
		}
	}

	// Assert
	TestEqual("Every (X, 0, Z) coordinate combination in the chunk should produce a unique index.",
		Indices.Num(), 64);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_HighestElevation_IsPositiveForPositiveChunkSpacing,
	"RDM.RDMMeshersTests.BaseVoxelData.HighestElevation.IsPositiveForPositiveChunkSpacing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_HighestElevation_IsPositiveForPositiveChunkSpacing::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;
	BaseVoxelDataDummy->VoxelSize = 1.0;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const double Elevation = BaseVoxelDataDummy->GetHighestElevationAtLocation(FVector::ZeroVector);

	// Assert
	TestTrue("Highest elevation should be positive when chunk spacing is positive.",
		Elevation > 0.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_XContributionIsLargerThanZContribution,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.XContributionIsLargerThanZContribution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_XContributionIsLargerThanZContribution::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 XStep = BaseVoxelDataDummy->CalculateVoxelIndex(1, 0, 0) - BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	const uint32 ZStep = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 1) - BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);

	// Assert
	TestTrue("A step along X should produce a larger index delta than a step along Z.",
		XStep > ZStep);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZContributionIsLargerThanYContribution,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.ZContributionIsLargerThanYContribution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZContributionIsLargerThanYContribution::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 ZStep = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 1) - BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	const uint32 YStep = BaseVoxelDataDummy->CalculateVoxelIndex(0, 1, 0) - BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);

	// Assert
	TestTrue("A step along Z should produce a larger index delta than a step along Y.",
		ZStep > YStep);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_AllVoxelsInFullChunkHaveUniqueIndices,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.AllVoxelsInFullChunkHaveUniqueIndices",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_AllVoxelsInFullChunkHaveUniqueIndices::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int32 Dim = 4;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = Dim;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	TSet<uint32> Indices;
	for (int32 X = 0; X < Dim; ++X)
		for (int32 Y = 0; Y < Dim; ++Y)
			for (int32 Z = 0; Z < Dim; ++Z)
				Indices.Add(BaseVoxelDataDummy->CalculateVoxelIndex(X, Y, Z));

	// Assert
	TestEqual("Every voxel coordinate in a full chunk should map to a unique index.",
		Indices.Num(), Dim * Dim * Dim);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_MaxIndexAcrossFullChunkEqualsMaxVoxelsMinusOne,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.MaxIndexAcrossFullChunkEqualsMaxVoxelsMinusOne",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_MaxIndexAcrossFullChunkEqualsMaxVoxelsMinusOne::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int32 Dim = 4;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = Dim;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	uint32 MaxFound = 0;
	for (int32 X = 0; X < Dim; ++X)
		for (int32 Y = 0; Y < Dim; ++Y)
			for (int32 Z = 0; Z < Dim; ++Z)
				MaxFound = FMath::Max(MaxFound, BaseVoxelDataDummy->CalculateVoxelIndex(X, Y, Z));

	// Assert
	TestEqual("The maximum index produced by valid coordinates should equal max voxels minus one.",
		MaxFound, BaseVoxelDataDummy->GetMaxVoxelsInChunk() - 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing_DoesNotChangeWhenOnlyVoxelCountChangesAndSizeIsZero,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing.DoesNotChangeWhenOnlyVoxelCountChangesAndSizeIsZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing_DoesNotChangeWhenOnlyVoxelCountChangesAndSizeIsZero::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelSize = 0.0;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 64;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Chunk spacing should remain zero regardless of dimension when voxel size is zero.",
		BaseVoxelDataDummy->GetChunkSpacing(), 0.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_FirstVoxelOnEachAxisFaceHasIndexLessThanChunkSize,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.FirstVoxelOnEachAxisFaceHasIndexLessThanChunkSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_FirstVoxelOnEachAxisFaceHasIndexLessThanChunkSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int32 Dim = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = Dim;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Max = BaseVoxelDataDummy->GetMaxVoxelsInChunk();

	// Act & Assert
	TestTrue("First voxel on the +X face should be inside the chunk.",
		BaseVoxelDataDummy->CalculateVoxelIndex(Dim - 1, 0, 0) < Max);
	TestTrue("First voxel on the +Y face should be inside the chunk.",
		BaseVoxelDataDummy->CalculateVoxelIndex(0, Dim - 1, 0) < Max);
	TestTrue("First voxel on the +Z face should be inside the chunk.",
		BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, Dim - 1) < Max);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelLine_ReflectsDefaultDimensionWithoutCallingCalculate,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelLine.ReflectsDefaultDimensionWithoutCallingCalculate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelLine_ReflectsDefaultDimensionWithoutCallingCalculate::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Voxel line after first calculate should match the default VoxelCountPerChunkDimension of 32.",
		BaseVoxelDataDummy->GetVoxelLine(), static_cast<uint32>(BaseVoxelDataDummy->VoxelCountPerChunkDimension));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_OriginHasSmallerIndexThanAllOtherVoxels,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.OriginHasSmallerIndexThanAllOtherVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_OriginHasSmallerIndexThanAllOtherVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int32 Dim = 4;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = Dim;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 OriginIndex = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);

	// Act
	bool bAllGreater = true;
	for (int32 X = 0; X < Dim && bAllGreater; ++X)
		for (int32 Y = 0; Y < Dim && bAllGreater; ++Y)
			for (int32 Z = 0; Z < Dim && bAllGreater; ++Z)
				if (X != 0 || Y != 0 || Z != 0)
					bAllGreater = BaseVoxelDataDummy->CalculateVoxelIndex(X, Y, Z) > OriginIndex;

	// Assert
	TestTrue("The origin voxel (0,0,0) should have a smaller index than every other voxel in the chunk.",
		bAllGreater);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing_HalvingDimensionHalvesSpacingWhenSizeIsFixed,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing.HalvingDimensionHalvesSpacingWhenSizeIsFixed",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing_HalvingDimensionHalvesSpacingWhenSizeIsFixed::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelSize = 2.0;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();
	const double FullSpacing = BaseVoxelDataDummy->GetChunkSpacing();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 16;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Halving the chunk dimension with fixed voxel size should halve the chunk spacing.",
		BaseVoxelDataDummy->GetChunkSpacing(), FullSpacing / 2.0);

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_SwappingXAndYProducesDifferentIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.SwappingXAndYProducesDifferentIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_SwappingXAndYProducesDifferentIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 IndexXY = BaseVoxelDataDummy->CalculateVoxelIndex(3, 7, 0);
	const uint32 IndexYX = BaseVoxelDataDummy->CalculateVoxelIndex(7, 3, 0);

	// Assert
	TestNotEqual("Swapping X and Y coordinates should produce a different index when X != Y.",
		IndexXY, IndexYX);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_SwappingXAndZProducesDifferentIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.SwappingXAndZProducesDifferentIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_SwappingXAndZProducesDifferentIndex::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 IndexXZ = BaseVoxelDataDummy->CalculateVoxelIndex(5, 0, 2);
	const uint32 IndexZX = BaseVoxelDataDummy->CalculateVoxelIndex(2, 0, 5);

	// Assert
	TestNotEqual("Swapping X and Z coordinates should produce a different index when X != Z.",
		IndexXZ, IndexZX);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_MaxVoxelsInChunk_GrowsCubicallyWithDimension,
	"RDM.RDMMeshersTests.BaseVoxelData.MaxVoxelsInChunk.GrowsCubicallyWithDimension",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_MaxVoxelsInChunk_GrowsCubicallyWithDimension::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 4;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 SmallChunk = BaseVoxelDataDummy->GetMaxVoxelsInChunk();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 LargeChunk = BaseVoxelDataDummy->GetMaxVoxelsInChunk();

	// Assert
	TestEqual("Doubling the dimension should multiply total voxel count by eight.",
		LargeChunk, SmallChunk * 8u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelSize_IsUnaffectedByVoxelCountChange,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelSize.IsUnaffectedByVoxelCountChange",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelSize_IsUnaffectedByVoxelCountChange::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelSize = 7.5;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 64;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Changing voxel count should not affect the stored voxel size.",
		BaseVoxelDataDummy->GetVoxelSize(), 7.5);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_OnlyMaxXVoxelHasHighestXContribution,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.OnlyMaxXVoxelHasHighestXContribution",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_OnlyMaxXVoxelHasHighestXContribution::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int32 Dim = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = Dim;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	const uint32 MaxXIndex = BaseVoxelDataDummy->CalculateVoxelIndex(Dim - 1, 0, 0);
	const uint32 SecondXIndex = BaseVoxelDataDummy->CalculateVoxelIndex(Dim - 2, 0, 0);

	// Assert
	TestTrue("The voxel at maximum X should have a larger index than the one before it.",
		MaxXIndex > SecondXIndex);
	TestEqual("The gap between max X and second X should equal voxel plane size.",
		MaxXIndex - SecondXIndex, BaseVoxelDataDummy->GetVoxelPlane());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing_WithSmallFractionalVoxelSizeIsAccurate,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing.WithSmallFractionalVoxelSizeIsAccurate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing_WithSmallFractionalVoxelSizeIsAccurate::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 100;
	BaseVoxelDataDummy->VoxelSize = 0.01;

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();

	// Assert
	TestEqual("Chunk spacing with small fractional voxel size should be computed accurately.",
		BaseVoxelDataDummy->GetChunkSpacing(), 1.0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_MultipleRecalculationsProduceSameIndexForSameInput,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.MultipleRecalculationsProduceSameIndexForSameInput",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_MultipleRecalculationsProduceSameIndexForSameInput::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 32;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 IndexBefore = BaseVoxelDataDummy->CalculateVoxelIndex(10, 5, 3);

	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 IndexAfter = BaseVoxelDataDummy->CalculateVoxelIndex(10, 5, 3);

	// Assert
	TestEqual("Calling CalculateVoxelData multiple times without changing inputs should not affect index output.",
		IndexAfter, IndexBefore);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelPlane_GrowsSquaredWithDimension,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlane.GrowsSquaredWithDimension",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelPlane_GrowsSquaredWithDimension::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 4;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 SmallPlane = BaseVoxelDataDummy->GetVoxelPlane();

	// Act
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 8;
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 LargePlane = BaseVoxelDataDummy->GetVoxelPlane();

	// Assert
	TestEqual("Doubling the dimension should multiply the voxel plane by four.",
		LargePlane, SmallPlane * 4u);

	return true;
}


