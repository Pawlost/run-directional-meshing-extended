#include "BaseVoxelData.h"
#include "Misc/AutomationTest.h"

class UBaseVoxelDataDummy : public UBaseVoxelData
{
public:
	using UBaseVoxelData::CalculateVoxelData; // workaround to access protected method as public
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelLineNumber,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlaneNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelLineNumber::RunTest(const FString& Parameters)
{
	// Prepare
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxel = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Voxel line should be set by VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetVoxelCountPerVoxelLine(), NumberOfVoxel);
	
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelPlaneNumber,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlaneNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelPlaneNumber::RunTest(const FString& Parameters)
{
	// Prepare
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxel = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Voxel plane should be power of 2 of VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetVoxelCountPerVoxelPlane(), NumberOfVoxel * NumberOfVoxel);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_ChunkSize,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_ChunkSize::RunTest(const FString& Parameters)
{
	// Prepare
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxel = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Chunk size should be power of 3 of VoxelCountPerChunkDimension member variable",
		BaseVoxelDataDummy->GetVoxelCountPerChunk(), NumberOfVoxel * NumberOfVoxel * NumberOfVoxel);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelSize,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelSize::RunTest(const FString& Parameters)
{
	// Prepare
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Check if VoxelSize is properly returned by GetVoxelSize function",
		BaseVoxelDataDummy->GetVoxelSize(), VoxelSize);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_ChunkSpacing,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkSpacing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_ChunkSpacing::RunTest(const FString& Parameters)
{
	// Prepare
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxel = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Chunk spacing should convert VoxelLine from virtual space to scene space",
		BaseVoxelDataDummy->GetChunkSpacing(), VoxelSize * NumberOfVoxel);
	
	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_ChunkIndexDirect,
	"RDM.RDMMeshersTests.BaseVoxelData.ChunkIndexDirect",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_ChunkIndexDirect::RunTest(const FString& Parameters)
{
	// Prepare 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxel = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	
	// Test
	TestEqual("Chunk spacing should convert VoxelLine from virtual space to scene space",
		BaseVoxelDataDummy->GetChunkSpacing(), VoxelSize * NumberOfVoxel);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelIndex_RandomDirectIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.RandomDirectIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelIndex_RandomDirectIndex::RunTest(const FString& Parameters)
{
	// Prepare 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxel = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 DirectIndex = BaseVoxelDataDummy->CalculateVoxelIndex(1, 2, 3);
	
	// Test
	TestEqual("Check if random voxel index return expected value",
		DirectIndex, 1122);
	
	return true;
}



IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData_VoxelIndex_RandomDirectIndex_Equals_OverloadedFIntVectorIndex,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelIndex.RandomDirectIndexEqualsOverloadedFIntVectorIndex",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTestsBaseVoxelData_VoxelIndex_RandomDirectIndex_Equals_OverloadedFIntVectorIndex::RunTest(const FString& Parameters)
{
	// Prepare 
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr double VoxelSize = 0.5;
	constexpr int NumberOfVoxel = 32;
	
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxel;
	BaseVoxelDataDummy->VoxelSize = VoxelSize;
	
	// Execute
	BaseVoxelDataDummy->CalculateVoxelData();
	const uint32 DirectIndex = BaseVoxelDataDummy->CalculateVoxelIndex(1, 2, 3);
	const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(FIntVector(1, 2, 3));
	
	// Test
	TestEqual("Check if random voxel index return expected value",
		Index, DirectIndex);
	
	
	return true;
}
