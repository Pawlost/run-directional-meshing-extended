#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMTestsExample,
	"RDMTests.Meshing.Example",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMTestsExample::RunTest(const FString& Parameters)
{
	TestTrue(TEXT("Initial unit test"), true);
	return true;
}