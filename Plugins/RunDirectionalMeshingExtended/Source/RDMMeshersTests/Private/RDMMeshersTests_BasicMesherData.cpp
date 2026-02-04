#include "Dummies/BaseVoxelDataDummy.h"
#include "Dummies/BasicMesherDataDummy.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_MaxPositionIndexTest,
	"RDM.RDMMeshersTests.BasicMesherData.MaxPositionIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_MaxPositionIndexTest::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	// Act
	const FIntVector VoxelPosition = FIntVector(NumberOfVoxels - 1, NumberOfVoxels - 1, NumberOfVoxels - 1);
	const uint32 VoxelIndex = BasicMesherDataDummy.CalculateIndexFromPosition(VoxelPosition);

	// Assert
	constexpr uint32 ExpectedMaxFor32VoxelCount = 32767u;
	TestEqual(TEXT("Index at maximum voxel position should equal max voxels in chunk minus one"), VoxelIndex,
	          ExpectedMaxFor32VoxelCount);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_ArbitraryPositionIndexCalculation,
	"RDM.RDMMeshersTests.BasicMesherData.ArbitraryPositionIndexCalculation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_ArbitraryPositionIndexCalculation::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	constexpr int X = 5;
	constexpr int Y = 7;
	constexpr int Z = 3;

	// Act
	const FIntVector VoxelPosition = FIntVector(X, Y, Z);
	const uint32 VoxelIndex = BasicMesherDataDummy.CalculateIndexFromPosition(VoxelPosition);

	// Assert
	constexpr uint32 ExpectedIndexFor32VoxelCount = 5223u;
	TestEqual(TEXT("Calculated voxel index should match expected plane/line/column index"), VoxelIndex,
	          ExpectedIndexFor32VoxelCount);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_ZeroOriginPositionReturnsZeroIndex,
	"RDM.RDMMeshersTests.BasicMesherData.ZeroOriginPositionReturnsZeroIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_ZeroOriginPositionReturnsZeroIndex::RunTest(const FString& Parameters)
{
	// Arrange
	FBasicMesherDataDummy BasicMesherDataNull(nullptr);

	// Act
	auto VoxelPosition = FIntVector(0, 0, 0);
	auto VoxelIndex = BasicMesherDataNull.CalculateIndexFromPosition(VoxelPosition);

	// Assert
	TestEqual(TEXT("Voxel index at origin should be zero"), VoxelIndex, 0u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_IndexPositionRoundTrip,
	"RDM.RDMMeshersTests.BasicMesherData.IndexPositionRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_IndexPositionRoundTrip::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	constexpr int X = 5;
	constexpr int Y = 7;
	constexpr int Z = 3;

	const auto OriginalPosition = FIntVector(X, Y, Z);

	// Act
	BasicMesherDataDummy.VoxelPosition = OriginalPosition;
	BasicMesherDataDummy.UpdateIndexFromPosition();
	const uint32 RecoveredIndex = BasicMesherDataDummy.VoxelIndex;
	BasicMesherDataDummy.UpdatePositionFromIndex();
	const FIntVector RecoveredPosition = BasicMesherDataDummy.VoxelPosition;

	// Assert
	constexpr uint32 ExpectedIndexFor32VoxelCount = 5223u;
	TestEqual(TEXT("Recovered index should match original index after position round-trip"), RecoveredIndex,
	          ExpectedIndexFor32VoxelCount);
	TestEqual(TEXT("Recovered position should match original position after index round-trip"), RecoveredPosition,
	          OriginalPosition);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_MaxIndexMapsToMaxPositionAndBack,
	"RDM.RDMMeshersTests.BasicMesherData.MaxIndexMapsToMaxPositionAndBack",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_MaxIndexMapsToMaxPositionAndBack::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	constexpr uint32 ExpectedMaxFor32VoxelCount = 32767u;
	const FIntVector ExpectedMaxPosition = FIntVector(NumberOfVoxels - 1, NumberOfVoxels - 1, NumberOfVoxels - 1);

	// Act
	BasicMesherDataDummy.VoxelIndex = ExpectedMaxFor32VoxelCount;
	BasicMesherDataDummy.UpdatePositionFromIndex();
	FIntVector RecoveredPosition = BasicMesherDataDummy.VoxelPosition;
	BasicMesherDataDummy.UpdateIndexFromPosition();
	const uint32 RecoveredIndex = BasicMesherDataDummy.VoxelIndex;

	// Assert
	TestEqual(TEXT("Position from max index should equal maximum voxel position"), RecoveredPosition,
	          ExpectedMaxPosition);
	TestEqual(TEXT("Index from maximum voxel position should equal max index"), RecoveredIndex,
	          ExpectedMaxFor32VoxelCount);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_IndexIncrementsAlongXByPlane,
	"RDM.RDMMeshersTests.BasicMesherData.IndexIncrementsAlongXByPlane",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_IndexIncrementsAlongXByPlane::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	// Act
	const uint32 IndexAtOrigin = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 0, 0));
	const uint32 IndexAtOneX = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(1, 0, 0));

	// Assert
	// X uses the plane stride = VoxelCount * VoxelCount
	constexpr uint32 ExpectedXStride = static_cast<uint32>(VoxelCount * VoxelCount);
	TestEqual(TEXT("Index should increase by voxel count squared when X increases by one"), IndexAtOneX,
	          IndexAtOrigin + ExpectedXStride);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_IndexIncrementsAlongYByStride,
	"RDM.RDMMeshersTests.BasicMesherData.IndexIncrementsAlongYByStride",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_IndexIncrementsAlongYByStride::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	// Act
	const uint32 IndexAtOrigin = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 0, 0));
	const uint32 IndexAtOneY = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 1, 0));

	// Assert
	constexpr uint32 ExpectedYStride = 1;
	TestEqual(TEXT("Index should increase by voxel count when Y increases by one"), IndexAtOneY,
	          IndexAtOrigin + ExpectedYStride);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_IndexIncrementsAlongZBySquareStride,
	"RDM.RDMMeshersTests.BasicMesherData.IndexIncrementsAlongZBySquareStride",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_IndexIncrementsAlongZBySquareStride::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	// Act
	const uint32 IndexAtOrigin = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 0, 0));
	const uint32 IndexAtOneZ = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 0, 1));

	// Assert
	constexpr uint32 ExpectedZStride = VoxelCount;
	TestEqual(TEXT("Index should increase by voxel count squared when Z increases by one"), IndexAtOneZ,
	          IndexAtOrigin + ExpectedZStride);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_VoxelCountOne_MaxIndexZero,
	"RDM.RDMMeshersTests.BasicMesherData.VoxelCountOne.MaxIndexZero",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_VoxelCountOne_MaxIndexZero::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 1;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	const uint32 MaxIndex = BasicMesherDataDummy.CalculateIndexFromPosition(FIntVector(0, 0, 0));
	TestEqual(TEXT("With a single voxel per dimension the only index should be zero"), MaxIndex, 0u);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_AllPositionsRoundTrip_SmallArea,
	"RDM.RDMMeshersTests.BasicMesherData.AllPositionsRoundTrip.SmallArea",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_AllPositionsRoundTrip_SmallArea::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = 2;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	constexpr uint32 MaxIndices = 2u * 2u * 2u;
	TSet<uint32> SeenIndices;

	for (int X = 0; X < 2; ++X)
	{
		for (int Y = 0; Y < 2; ++Y)
		{
			for (int Z = 0; Z < 2; ++Z)
			{
				const FIntVector Pos(X, Y, Z);
				const uint32 Index = BasicMesherDataDummy.CalculateIndexFromPosition(Pos);
				TestTrue(
					*FString::Printf(TEXT("Index for position (%d,%d,%d) must be less than total voxels"), X, Y, Z),
					Index < MaxIndices);

				TestFalse(*FString::Printf(TEXT("Index for position (%d,%d,%d) must be unique"), X, Y, Z),
				          SeenIndices.Contains(Index));
				SeenIndices.Add(Index);

				// Round-trip: position -> index -> position
				BasicMesherDataDummy.VoxelPosition = Pos;
				BasicMesherDataDummy.UpdateIndexFromPosition();
				const uint32 RoundIndex = BasicMesherDataDummy.VoxelIndex;
				BasicMesherDataDummy.UpdatePositionFromIndex();
				const FIntVector RoundPos = BasicMesherDataDummy.VoxelPosition;
				TestEqual(*FString::Printf(
					          TEXT("Round-trip for position (%d,%d,%d) must recover the same index"), X, Y, Z),
				          RoundIndex, Index);
				TestEqual(*FString::Printf(
					          TEXT("Round-trip for position (%d,%d,%d) must recover the same position"), X, Y, Z),
				          RoundPos, Pos);
			}
		}
	}

	TestEqual(TEXT("Should have produced exactly eight unique indices for 2^3 positions"), SeenIndices.Num(),
	          MaxIndices);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_AllPositionsRoundTrip_AllBoundsPositions,
	"RDM.RDMMeshersTests.BasicMesherData.AllPositionsRoundTrip.AllBoundsPositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_AllPositionsRoundTrip_AllBoundsPositions::RunTest(const FString& Parameters)
{
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

	constexpr uint32 Total = static_cast<uint32>(VoxelCount * VoxelCount * VoxelCount);
	TSet<uint32> SeenIndices;

	for (int X = 0; X < VoxelCount; ++X)
	{
		for (int Y = 0; Y < VoxelCount; ++Y)
		{
			for (int Z = 0; Z < VoxelCount; ++Z)
			{
				const FIntVector Pos(X, Y, Z);
				const uint32 Index = BasicMesherDataDummy.CalculateIndexFromPosition(Pos);
				TestTrue(*FString::Printf(TEXT("Index for (%d,%d,%d) must be within bounds"), X, Y, Z), Index < Total);
				SeenIndices.Add(Index);

				BasicMesherDataDummy.VoxelPosition = Pos;
				BasicMesherDataDummy.UpdateIndexFromPosition();
				const uint32 RoundIndex = BasicMesherDataDummy.VoxelIndex;
				BasicMesherDataDummy.UpdatePositionFromIndex();
				const FIntVector RoundPos = BasicMesherDataDummy.VoxelPosition;
				TestEqual(*FString::Printf(
					          TEXT("Round-trip position for (%d,%d,%d) recovers original position"), X, Y, Z), RoundPos,
				          Pos);
				TestEqual(*FString::Printf(TEXT("Round-trip index for (%d,%d,%d) recovers original index"), X, Y, Z),
				          RoundIndex, Index);
			}
		}
	}

	TestEqual(TEXT("Number of unique indices for 8^3 positions should equal total voxels"), SeenIndices.Num(), Total);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherData_MultipleVoxelCounts,
	"RDM.RDMMeshersTests.BasicMesherData.MultipleVoxelCounts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherData_MultipleVoxelCounts::RunTest(const FString& Parameters)
{
	TArray<int> Counts = {2, 4, 8, 16};
	for (int Count : Counts)
	{
		UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
		BaseVoxelDataDummy->VoxelCountPerChunkDimension = Count;
		BaseVoxelDataDummy->CalculateVoxelData();

		FBasicMesherDataDummy BasicMesherDataDummy(BaseVoxelDataDummy);

		const uint32 Total = static_cast<uint32>(Count * Count * Count);
		TArray<FIntVector> Samples;
		Samples.Add(FIntVector(0, 0, 0));
		Samples.Add(FIntVector(Count - 1, 0, 0));
		Samples.Add(FIntVector(0, Count - 1, 0));
		Samples.Add(FIntVector(0, 0, Count - 1));
		Samples.Add(FIntVector(Count - 1, Count - 1, Count - 1));
		Samples.Add(FIntVector(Count / 2, Count / 2, Count / 2));

		for (const FIntVector& Pos : Samples)
		{
			const uint32 Index = BasicMesherDataDummy.CalculateIndexFromPosition(Pos);
			TestTrue(*FString::Printf(
				         TEXT("Index for (%d,%d,%d) with count %d must be < total"), Pos.X, Pos.Y, Pos.Z, Count),
			         Index < Total);

			BasicMesherDataDummy.VoxelPosition = Pos;
			BasicMesherDataDummy.UpdateIndexFromPosition();
			const uint32 RoundIndex = BasicMesherDataDummy.VoxelIndex;
			BasicMesherDataDummy.UpdatePositionFromIndex();
			const FIntVector RoundPos = BasicMesherDataDummy.VoxelPosition;
			TestEqual(*FString::Printf(
				          TEXT("Round-trip position for (%d,%d,%d) with count %d"), Pos.X, Pos.Y, Pos.Z, Count),
			          RoundPos, Pos);
			TestEqual(*FString::Printf(
				          TEXT("Round-trip index for (%d,%d,%d) with count %d"), Pos.X, Pos.Y, Pos.Z, Count),
			          RoundIndex, Index);
		}
	}

	return true;
}
