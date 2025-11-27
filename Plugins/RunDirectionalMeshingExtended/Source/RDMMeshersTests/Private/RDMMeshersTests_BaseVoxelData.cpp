#include "BaseVoxelData.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTestsBaseVoxelData,
	"RDM.RDMMeshersTests.BaseVoxelData.Example",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

class UBaseVoxelDataDummy : public UBaseVoxelData
{
public:
	using UBaseVoxelData::CalculateVoxelData; // workaround to access protected method as public
};

bool FRDMMeshersTestsBaseVoxelData::RunTest(const FString& Parameters)
{
	// Construct the UObject
	UBaseVoxelDataDummy* TestBaseVoxelData = NewObject<UBaseVoxelDataDummy>();
	constexpr int NumberOfVoxel = 16;
	
	TestBaseVoxelData->VoxelCountPerChunkDimension = NumberOfVoxel;
	TestBaseVoxelData->CalculateVoxelData();
	
	TestEqual("Voxel plane should be power of 2 compared to initial set",
		TestBaseVoxelData->GetVoxelCountPerVoxelPlane(), NumberOfVoxel * NumberOfVoxel);
	
	return true;
}