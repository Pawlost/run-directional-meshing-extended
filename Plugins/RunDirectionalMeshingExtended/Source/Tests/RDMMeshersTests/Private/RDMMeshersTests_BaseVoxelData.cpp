#include "BaseVoxelData.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelLineNumber,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelLineNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelLineNumber::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Voxel line should be set by VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetVoxelLine(), NumberOfVoxels);
	
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelPlaneNumber,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlaneNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelPlaneNumber::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Voxel plane should be power of 2 of VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetVoxelPlane(), NumberOfVoxels * NumberOfVoxels);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSize,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Chunk size should be power of 3 of VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetMaxVoxelsInChunk(), NumberOfVoxels * NumberOfVoxels * NumberOfVoxels);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelSize,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Check if VoxelSize is properly returned by GetVoxelSize function",
		BaseVoxelDataDummy->GetVoxelSize(), VoxelSize);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkSpacing,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkSpacing::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Chunk spacing should convert VoxelLine from virtual space to scene space",
		BaseVoxelDataDummy->GetChunkSpacing(), VoxelSize * NumberOfVoxels);
	
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_ChunkIndexDirect,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkIndexDirect",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_ChunkIndexDirect::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Assert
	TestEqual("Chunk spacing should convert VoxelLine from virtual space to scene space",
		BaseVoxelDataDummy->GetChunkSpacing(), VoxelSize * NumberOfVoxels);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_RandomDirectIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.RandomDirectIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_RandomDirectIndex::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 DirectIndex = BaseVoxelDataDummy->CalculateVoxelIndex(1, 2, 3);
	
	// Assert
	TestEqual("Check if random voxel index return expected value",
		DirectIndex, 1122);
	
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_RandomDirectIndex_Equals_OverloadedFIntVectorIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.RandomDirectIndexEqualsOverloadedFIntVectorIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_RandomDirectIndex_Equals_OverloadedFIntVectorIndex::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 DirectIndex = BaseVoxelDataDummy->CalculateVoxelIndex(1, 2, 3);
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(FIntVector(1, 2, 3));
	
	// Assert
	TestEqual("Check if random voxel index return expected value",
		Index, DirectIndex);
	
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZeroIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.ZeroIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_ZeroIndex::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	
	// Assert
	TestEqual("Zero position is the first index. Index value should be 0.",
		Index, 0);
	
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_LastIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.LastIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_LastIndex::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(NumberOfVoxels - 1, NumberOfVoxels - 1, NumberOfVoxels - 1);

	// Assert
	TestEqual("Last index should be less than maximum number of voxels in a chunk.",
		Index, BaseVoxelDataDummy->GetMaxVoxelsInChunk() - 1);
	
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexIsOutOfChunkBounds,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.IndexIsOutOfChunkBounds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_BaseVoxelData_VoxelIndex_IndexIsOutOfChunkBounds::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 InvalidIndex = BaseVoxelDataDummy->CalculateVoxelIndex(NumberOfVoxels + 1, NumberOfVoxels + 1, NumberOfVoxels + 1);
	
	// Assert
	TestTrue("Out of bounds index is ignored if it is higher than what function GetVoxelCountPerChunk() returns.",
		InvalidIndex > BaseVoxelDataDummy->GetMaxVoxelsInChunk());
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelIndex_NotNegative,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.NotNegative",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelIndex_NotNegative::RunTest(const FString& Parameters)
{
	// Arrange 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	// Act
	BaseVoxelDataDummy->CalculateVoxelData();
	const auto NegativeIndex = BaseVoxelDataDummy->CalculateVoxelIndex(0, -1, 0);
	
	// Assert
	TestLessThan("Index shouldn't be a negative number. Only positive.",
		NegativeIndex, 0);
	
	return true;
}
