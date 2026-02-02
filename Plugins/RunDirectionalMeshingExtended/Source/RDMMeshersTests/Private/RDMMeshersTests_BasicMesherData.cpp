#include "Dummies/BaseVoxelDataDummy.h"
#include "Misc/AutomationTest.h"
#include "VoxelMesher/BasicMesherData.h"

/*
struct FBasicMesherDataDummy : public FBasicMesherData
{
public:
	FBasicMesherDataDummy(const uint32 VoxelLine, const uint32 VoxelPlane, const uint32 MaxVoxelsInChunk)
		: FBasicMesherData(VoxelLine, VoxelPlane, MaxVoxelsInChunk)
	{
	}
	
	// workaround to access protected method as public
	using FBasicMesherData::UpdatePositionFromIndex; 
	using FBasicMesherData::UpdateIndexFromPosition; 
	using FBasicMesherData::CalculateIndexFromPosition; 
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicMesherDataTest,
	"RDM.RDMMeshersTests.BasicMesherData.VoxelPlaneNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicMesherDataTest::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxels = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = NumberOfVoxels;
	
	FBasicMesherDataDummy BasicMesherDataDummy = FBasicMesherDataDummy(32,32,32);
	
	// Act
	auto VoxelIndex = BasicMesherDataDummy.CalculateIndexFromPosition();
	
	// Assert
	//TestEqual(VoxelIndex);
	
	return true;
*/