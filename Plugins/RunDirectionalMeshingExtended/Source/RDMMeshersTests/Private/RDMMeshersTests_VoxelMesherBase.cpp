#include "Misc/AutomationTest.h"
#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

class UDummyRLERunDirectionalVoxelMesher : public URLERunDirectionalVoxelMesher
{
public:
	using URLERunDirectionalVoxelMesher::AdvanceEditInterval;
};


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersRLERunDirectionalVoxelMesher,
	"RDM.RDMMeshersTests.BaseVoxelData.VoxelPlaneNumber",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersRLERunDirectionalVoxelMesher::RunTest(const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* BaseVoxelDataDummy = NewObject<UDummyRLERunDirectionalVoxelMesher>();
	
	// Execute
	
	// Test
	
	return true;
}
