#include "Misc/AutomationTest.h"
#include "Dummies/BaseVoxelDataDummy.h"
#include "VirtualChunk/RLEVirtualChunk.h"
#include "VirtualChunk/ChunkBorderContext.h"
#include "VoxelMesher/BorderVoxelEventMesher.h"
#include "VoxelMesher/MeshingUtil/RLEVoxelEdit.h"
#include "VoxelMesher/MeshingUtil/VoxelMesh.h"

namespace
{
	UBaseVoxelDataDummy* CreateVoxelData(const int64 ChunkDimension)
	{
		UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
		BaseVoxelDataDummy->VoxelCountPerChunkDimension = ChunkDimension;
		BaseVoxelDataDummy->VoxelSize = 1.0;
		BaseVoxelDataDummy->CalculateVoxelData();
		return BaseVoxelDataDummy;
	}

	URLEVirtualChunk* CreateRLEChunk(UBaseVoxelDataDummy* BaseVoxelDataDummy)
	{
		URLEVirtualChunk* RLEChunk = NewObject<URLEVirtualChunk>();
		RLEChunk->SetVoxelGenerator(BaseVoxelDataDummy);
		return RLEChunk;
	}

	TArray<FVoxel> CreateFilledVoxelGrid(const UBaseVoxelDataDummy* BaseVoxelDataDummy, const FVoxel& FillVoxel)
	{
		TArray<FVoxel> VoxelGrid;
		VoxelGrid.Init(FillVoxel, static_cast<int32>(BaseVoxelDataDummy->GetMaxVoxelsInChunk()));
		return VoxelGrid;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_EmptyInputThenValidInput_UsesValidInputForSampling,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.EmptyInputThenValidInputUsesValidInputForSampling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_EmptyInputThenValidInput_UsesValidInputForSampling::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyVoxelGrid;
	TArray<FVoxel> SolidVoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(7));
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(EmptyVoxelGrid);
	RLEChunk->CompressVoxelModel(SolidVoxelGrid);
	const FVoxel SampledVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 1, 1));

	// Assert
	TestEqual("Sampling after empty then valid compression should return voxel from valid input.",
		SampledVoxel.VoxelId, 7);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_WhenAlreadyCompressed_DoesNotReplaceExistingData,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.WhenAlreadyCompressedDoesNotReplaceExistingData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_WhenAlreadyCompressed_DoesNotReplaceExistingData::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> FirstVoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(3));
	TArray<FVoxel> SecondVoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(9));
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(FirstVoxelGrid);
	RLEChunk->CompressVoxelModel(SecondVoxelGrid);
	const FVoxel SampledVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 1, 1));

	// Assert
	TestEqual("Second compression call should not replace already compressed voxel data.",
		SampledVoxel.VoxelId, 3);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SequentialSamplingAcrossRunBoundary_ReturnsExpectedVoxels,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.SequentialSamplingAcrossRunBoundaryReturnsExpectedVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SequentialSamplingAcrossRunBoundary_ReturnsExpectedVoxels::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(2));
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 1)] = FVoxel(5);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(0, 1, 1)] = FVoxel(5);
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FBorderEventMesher BorderEventMesher;

	// Act
	const FVoxel FirstRunVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 1, 0));
	const FVoxel SecondRunVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 1));

	// Assert
	TestEqual("Sampling inside the first run should return the first voxel id.",
		FirstRunVoxel.VoxelId, 2);
	TestEqual("Sampling after crossing run boundary should return the next voxel id.",
		SecondRunVoxel.VoxelId, 5);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SamplingSamePositionTwice_ReturnsSameVoxel,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.SamplingSamePositionTwiceReturnsSameVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SamplingSamePositionTwice_ReturnsSameVoxel::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(4));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FBorderEventMesher BorderEventMesher;

	// Act
	const FVoxel FirstSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 0, 1));
	const FVoxel SecondSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 0, 1));

	// Assert
	TestEqual("First sample should match compressed voxel id.",
		FirstSample.VoxelId, 4);
	TestEqual("Sampling the same position again should return the same voxel id.",
		SecondSample.VoxelId, FirstSample.VoxelId);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithBorderVisualizationAll_ProducesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.AllSolidVoxelsWithBorderVisualizationAllProducesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithBorderVisualizationAll_ProducesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(8));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestTrue("Solid chunk with full border visualization should generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllEmptyVoxels_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.AllEmptyVoxelsProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllEmptyVoxels_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Empty chunk should not generate any mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_WithEditThatAddsSolidVoxel_ProducesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.WithEditThatAddsSolidVoxelProducesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_WithEditThatAddsSolidVoxel_ProducesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(0, 1, 0), FRLEVoxel{1, FVoxel(6)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestTrue("Applying edit that adds one solid voxel should generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_SingleTransparentVoxel_PreservesTransparencyOnSampling,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.SingleTransparentVoxelPreservesTransparencyOnSampling",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_SingleTransparentVoxel_PreservesTransparencyOnSampling::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(1);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(0), true));
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	const FVoxel SampledVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));

	// Assert
	TestTrue("Sampled voxel should remain transparent.",
		SampledVoxel.IsTransparent());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SamplingWithSeparatePlannersAtSamePosition_ReturnsEqualValues,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.SamplingWithSeparatePlannersAtSamePositionReturnsEqualValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SamplingWithSeparatePlannersAtSamePosition_ReturnsEqualValues::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(17));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FBorderEventMesher FirstPlanner;
	FBorderEventMesher SecondPlanner;

	// Act
	const FVoxel FirstPlannerSample = RLEChunk->GetBorderVoxel(FirstPlanner, FIntVector(1, 1, 0));
	const FVoxel SecondPlannerSample = RLEChunk->GetBorderVoxel(SecondPlanner, FIntVector(1, 1, 0));

	// Assert
	TestEqual("Different planners should return the same voxel value for the same position.",
		FirstPlannerSample.VoxelId, SecondPlannerSample.VoxelId);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditThenGenerateAgain_SecondGenerationKeepsEditedVoxelValue,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.EditThenGenerateAgainSecondGenerationKeepsEditedVoxelValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditThenGenerateAgain_SecondGenerationKeepsEditedVoxelValue::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> FirstEdits;
	FirstEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(1, 0, 1), FRLEVoxel{1, FVoxel(25)}});
	TArray<FRLEVoxelEdit> SecondEdits;
	FVoxelMesh FirstMeshContainer;
	FVoxelMesh SecondMeshContainer;

	// Act
	RLEChunk->GenerateMesh(FirstMeshContainer, BorderContext, FirstEdits);
	RLEChunk->GenerateMesh(SecondMeshContainer, BorderContext, SecondEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel EditedVoxelSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 0, 1));

	// Assert
	TestEqual("First edit list should be consumed.",
		FirstEdits.Num(), 0);
	TestEqual("Second generation without edits should keep edited voxel value.",
		EditedVoxelSample.VoxelId, 25);
	TestTrue("Second generation should still contain mesh sections from persistent edited voxel.",
		SecondMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_SingleSolidVoxelEditedToEmpty_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.SingleSolidVoxelEditedToEmptyProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_SingleSolidVoxelEditedToEmpty_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(1);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(15));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0), FRLEVoxel{1, FVoxel(FVoxel::EMPTY_VOXEL)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Editing the only solid voxel to empty should remove all mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_MixedVoxelKinds_SamplesExpectedKinds,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.MixedVoxelKindsSamplesExpectedKinds",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_MixedVoxelKinds_SamplesExpectedKinds::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(0, 1, 0)] = FVoxel(static_cast<uint32>(0), true);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 1)] = FVoxel(9);
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	const FVoxel EmptySample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));
	const FVoxel TransparentSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 1, 0));
	const FVoxel SolidSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 1));

	// Assert
	TestTrue("First sample should be empty voxel.",
		EmptySample.IsEmptyVoxel());
	TestTrue("Second sample should be transparent voxel.",
		TransparentSample.IsTransparent());
	TestEqual("Third sample should match expected solid voxel id.",
		SolidSample.VoxelId, 9);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_FullChunkSequentialSampling_MatchesSourceGrid,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.FullChunkSequentialSamplingMatchesSourceGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_FullChunkSequentialSampling_MatchesSourceGrid::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(static_cast<int32>(BaseVoxelDataDummy->GetMaxVoxelsInChunk()));
	for (int32 X = 0; X < 2; ++X)
	{
		for (int32 Z = 0; Z < 2; ++Z)
		{
			for (int32 Y = 0; Y < 2; ++Y)
			{
				const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(X, Y, Z);
				VoxelGrid[static_cast<int32>(Index)] = FVoxel(static_cast<int32>(Index) + 1);
			}
		}
	}
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	bool bAllSamplesMatch = true;
	for (int32 X = 0; X < 2 && bAllSamplesMatch; ++X)
	{
		for (int32 Z = 0; Z < 2 && bAllSamplesMatch; ++Z)
		{
			for (int32 Y = 0; Y < 2 && bAllSamplesMatch; ++Y)
			{
				const uint32 Index = BaseVoxelDataDummy->CalculateVoxelIndex(X, Y, Z);
				const FVoxel SampledVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(X, Y, Z));
				bAllSamplesMatch = SampledVoxel.VoxelId == VoxelGrid[static_cast<int32>(Index)].VoxelId;
			}
		}
	}

	// Assert
	TestTrue("Sequential border sampling should match all source voxel values.",
		bAllSamplesMatch);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithNoBorderVisualization_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.AllSolidVoxelsWithNoBorderVisualizationProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithNoBorderVisualization_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(7));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::None;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Solid chunk with no border visualization should not generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithOnlyOuterBorders_ProducesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.AllSolidVoxelsWithOnlyOuterBordersProducesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_AllSolidVoxelsWithOnlyOuterBorders_ProducesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(7));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestTrue("Solid chunk with only outer borders visualization should generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TwoEditsAtDifferentPositions_AppliesBothAndConsumesEdits,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.TwoEditsAtDifferentPositionsAppliesBothAndConsumesEdits",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TwoEditsAtDifferentPositions_AppliesBothAndConsumesEdits::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector FirstEditedPosition(0, 1, 0);
	const FIntVector SecondEditedPosition(1, 0, 1);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(FirstEditedPosition), FRLEVoxel{1, FVoxel(31)}});
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(SecondEditedPosition), FRLEVoxel{1, FVoxel(32)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel FirstEditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FirstEditedPosition);
	const FVoxel SecondEditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, SecondEditedPosition);

	// Assert
	TestEqual("All edits should be consumed by mesh generation.",
		VoxelEdits.Num(), 0);
	TestEqual("First edited position should contain first edited voxel id.",
		FirstEditedSample.VoxelId, 31);
	TestEqual("Second edited position should contain second edited voxel id.",
		SecondEditedSample.VoxelId, 32);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_AllTransparentVoxels_SamplesStayTransparent,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.AllTransparentVoxelsSamplesStayTransparent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_AllTransparentVoxels_SamplesStayTransparent::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> TransparentGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(2), true));
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(TransparentGrid);
	const FVoxel FirstSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));
	const FVoxel LastSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 1, 1));

	// Assert
	TestTrue("First sampled voxel should be transparent.",
		FirstSample.IsTransparent());
	TestTrue("Last sampled voxel should also be transparent.",
		LastSample.IsTransparent());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_OnlyOuterBordersWithEmptyChunk_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.OnlyOuterBordersWithEmptyChunkProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_OnlyOuterBordersWithEmptyChunk_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Empty chunk should not generate mesh sections when only outer borders are enabled.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TransparentBaseWithSolidEdit_ProducesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.TransparentBaseWithSolidEditProducesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TransparentBaseWithSolidEdit_ProducesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> TransparentGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(0), true));
	RLEChunk->CompressVoxelModel(TransparentGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector EditedPosition(1, 1, 0);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(EditedPosition), FRLEVoxel{1, FVoxel(41)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel EditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, EditedPosition);

	// Assert
	TestEqual("Edited position should store the new solid voxel value.",
		EditedSample.VoxelId, 41);
	TestTrue("Applying a solid edit into a transparent chunk should generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_SolidBaseWithTransparentEdit_PersistsTransparentVoxel,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.SolidBaseWithTransparentEditPersistsTransparentVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_SolidBaseWithTransparentEdit_PersistsTransparentVoxel::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> SolidGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(12));
	RLEChunk->CompressVoxelModel(SolidGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector EditedPosition(0, 0, 1);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(EditedPosition), FRLEVoxel{1, FVoxel(static_cast<uint32>(3), true)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel EditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, EditedPosition);

	// Assert
	TestTrue("Edited voxel should be transparent after mesh generation.",
		EditedSample.IsTransparent());
	TestTrue("Chunk should still generate mesh sections after one voxel becomes transparent.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_IsolatedSolidVoxelInEmptyChunk_SamplingFindsOnlyIsolatedSolid,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.IsolatedSolidVoxelInEmptyChunkSamplingFindsOnlyIsolatedSolid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_IsolatedSolidVoxelInEmptyChunk_SamplingFindsOnlyIsolatedSolid::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	const FIntVector IsolatedSolidPosition(1, 0, 1);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(IsolatedSolidPosition)] = FVoxel(77);
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	const FVoxel EmptySample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));
	const FVoxel SolidSample = RLEChunk->GetBorderVoxel(BorderEventMesher, IsolatedSolidPosition);

	// Assert
	TestTrue("Voxel sampled away from isolated solid should remain empty.",
		EmptySample.IsEmptyVoxel());
	TestEqual("Voxel sampled at isolated position should match the solid voxel id.",
		SolidSample.VoxelId, 77);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditSolidThenEditBackToEmpty_SecondCallRemovesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.EditSolidThenEditBackToEmptySecondCallRemovesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditSolidThenEditBackToEmpty_SecondCallRemovesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(1);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	FVoxelMesh FirstMeshContainer;
	FVoxelMesh SecondMeshContainer;
	TArray<FRLEVoxelEdit> FirstEdits;
	TArray<FRLEVoxelEdit> SecondEdits;
	const uint32 OriginIndex = BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0);
	FirstEdits.Add(FRLEVoxelEdit{OriginIndex, FRLEVoxel{1, FVoxel(22)}});
	SecondEdits.Add(FRLEVoxelEdit{OriginIndex, FRLEVoxel{1, FVoxel(FVoxel::EMPTY_VOXEL)}});

	// Act
	RLEChunk->GenerateMesh(FirstMeshContainer, BorderContext, FirstEdits);
	RLEChunk->GenerateMesh(SecondMeshContainer, BorderContext, SecondEdits);

	// Assert
	TestTrue("First edit turning voxel solid should generate mesh sections.",
		FirstMeshContainer.VoxelTable.Num() > 0);
	TestEqual("Second edit turning voxel back to empty should remove mesh sections.",
		SecondMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TransparentEditOnEmptyChunk_DoesNotCreateMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.TransparentEditOnEmptyChunkDoesNotCreateMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_TransparentEditOnEmptyChunk_DoesNotCreateMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	FVoxelMesh VoxelMeshContainer;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(1, 1, 0), FRLEVoxel{1, FVoxel(static_cast<uint32>(5), true)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Changing empty voxel to transparent voxel should still produce no mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_MultipleEditsConsumedThenNoEditsRemain_NoFurtherEditConsumptionNeeded,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.MultipleEditsConsumedThenNoEditsRemainNoFurtherEditConsumptionNeeded",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_MultipleEditsConsumedThenNoEditsRemain_NoFurtherEditConsumptionNeeded::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0), FRLEVoxel{1, FVoxel(61)}});
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(1, 1, 1), FRLEVoxel{1, FVoxel(62)}});
	FVoxelMesh FirstMeshContainer;
	FVoxelMesh SecondMeshContainer;
	TArray<FRLEVoxelEdit> NoEdits;

	// Act
	RLEChunk->GenerateMesh(FirstMeshContainer, BorderContext, VoxelEdits);
	RLEChunk->GenerateMesh(SecondMeshContainer, BorderContext, NoEdits);

	// Assert
	TestEqual("All pending edits should be consumed during first generation call.",
		VoxelEdits.Num(), 0);
	TestEqual("Subsequent generation with empty edit list should keep it empty.",
		NoEdits.Num(), 0);
	TestTrue("Mesh sections should persist after edits were applied and consumed.",
		SecondMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_TerminalTransparentVoxel_SamplesAsTransparent,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.TerminalTransparentVoxelSamplesAsTransparent",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_TerminalTransparentVoxel_SamplesAsTransparent::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(15));
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(1, 1, 1)] = FVoxel(static_cast<uint32>(4), true);
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	const FVoxel OriginSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));
	const FVoxel TerminalSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 1, 1));

	// Assert
	TestEqual("Origin sample should keep base solid voxel id.",
		OriginSample.VoxelId, 15);
	TestTrue("Terminal sample should be transparent.",
		TerminalSample.IsTransparent());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_DescendingIndexRequestAfterAdvance_ReturnsCurrentAdvancedVoxel,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.DescendingIndexRequestAfterAdvanceReturnsCurrentAdvancedVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_DescendingIndexRequestAfterAdvance_ReturnsCurrentAdvancedVoxel::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(3));
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(1, 1, 0)] = FVoxel(8);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(1, 1, 1)] = FVoxel(9);
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FBorderEventMesher BorderEventMesher;

	// Act
	const FVoxel HighIndexSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 1, 1));
	const FVoxel LowerIndexSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(0, 0, 0));

	// Assert
	TestEqual("High index sample should match expected terminal voxel id.",
		HighIndexSample.VoxelId, 9);
	TestEqual("Sampling lower index with same planner should keep current advanced voxel.",
		LowerIndexSample.VoxelId, HighIndexSample.VoxelId);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoEditsForSolidChunk_LeavesEditArrayEmpty,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.NoEditsForSolidChunkLeavesEditArrayEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoEditsForSolidChunk_LeavesEditArrayEmpty::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(24));
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("No incoming edits should keep edit array empty after meshing.",
		VoxelEdits.Num(), 0);
	TestTrue("Solid chunk should generate mesh sections.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_RemoveOnlySolidVoxelFromSparseChunk_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.RemoveOnlySolidVoxelFromSparseChunkProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_RemoveOnlySolidVoxelFromSparseChunk_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	const FIntVector SolidPosition(1, 0, 1);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(SolidPosition)] = FVoxel(42);
	RLEChunk->CompressVoxelModel(VoxelGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(SolidPosition), FRLEVoxel{1, FVoxel(FVoxel::EMPTY_VOXEL)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Removing the only solid voxel should result in no mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);
	TestEqual("Edit list should be consumed after mesh generation.",
		VoxelEdits.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_UniformTransparentGrid_SampledVoxelIsTransparentAndNotEmpty,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.UniformTransparentGridSampledVoxelIsTransparentAndNotEmpty",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_UniformTransparentGrid_SampledVoxelIsTransparentAndNotEmpty::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> TransparentGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(7), true));
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(TransparentGrid);
	const FVoxel SampledVoxel = RLEChunk->GetBorderVoxel(BorderEventMesher, FIntVector(1, 0, 1));

	// Assert
	TestTrue("Sampled voxel should be transparent for a uniform transparent grid.",
		SampledVoxel.IsTransparent());
	TestTrue("Transparent voxel should not be treated as empty.",
		!SampledVoxel.IsEmptyVoxel());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoBorderVisualizationWithTransparentGridAndSolidEdit_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.NoBorderVisualizationWithTransparentGridAndSolidEditProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoBorderVisualizationWithTransparentGridAndSolidEdit_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> TransparentGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(2), true));
	RLEChunk->CompressVoxelModel(TransparentGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::None;
	TArray<FRLEVoxelEdit> VoxelEdits;
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(0, 1, 0), FRLEVoxel{1, FVoxel(45)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("No border visualization should prevent mesh sections even after a solid edit.",
		VoxelMeshContainer.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_OnlyOuterBordersWithTransparentGridAndSolidEdit_ProducesMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.OnlyOuterBordersWithTransparentGridAndSolidEditProducesMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_OnlyOuterBordersWithTransparentGridAndSolidEdit_ProducesMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> TransparentGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(static_cast<uint32>(2), true));
	RLEChunk->CompressVoxelModel(TransparentGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector EditedPosition(1, 1, 0);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(EditedPosition), FRLEVoxel{1, FVoxel(46)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel EditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, EditedPosition);

	// Assert
	TestEqual("Edited solid voxel should persist in voxel sampling after mesh generation.",
		EditedSample.VoxelId, 46);
	TestTrue("Only outer borders mode should produce mesh sections for inserted solid voxel.",
		VoxelMeshContainer.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoEditsWithOnlyOuterBordersOnEmptyGrid_ProducesNoMeshSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.NoEditsWithOnlyOuterBordersOnEmptyGridProducesNoMeshSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoEditsWithOnlyOuterBordersOnEmptyGrid_ProducesNoMeshSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;
	TArray<FRLEVoxelEdit> VoxelEdits;

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);

	// Assert
	TestEqual("Empty grid with only outer borders and no edits should produce no mesh sections.",
		VoxelMeshContainer.VoxelTable.Num(), 0);
	TestEqual("Edit list should stay empty when no edits are provided.",
		VoxelEdits.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_SparseSolidVoxelsAtBothEnds_SamplesBothEndsAndMiddle,
	"RDM.RDMMeshersTests.RLEVirtualChunk.CompressVoxelModel.SparseSolidVoxelsAtBothEndsSamplesBothEndsAndMiddle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_CompressVoxelModel_SparseSolidVoxelsAtBothEnds_SamplesBothEndsAndMiddle::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> VoxelGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	const FIntVector FirstPosition(0, 0, 0);
	const FIntVector MiddlePosition(0, 1, 0);
	const FIntVector LastPosition(1, 1, 1);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(FirstPosition)] = FVoxel(10);
	VoxelGrid[BaseVoxelDataDummy->CalculateVoxelIndex(LastPosition)] = FVoxel(20);
	FBorderEventMesher BorderEventMesher;

	// Act
	RLEChunk->CompressVoxelModel(VoxelGrid);
	const FVoxel FirstSample = RLEChunk->GetBorderVoxel(BorderEventMesher, FirstPosition);
	const FVoxel MiddleSample = RLEChunk->GetBorderVoxel(BorderEventMesher, MiddlePosition);
	const FVoxel LastSample = RLEChunk->GetBorderVoxel(BorderEventMesher, LastPosition);

	// Assert
	TestEqual("First sample should match first configured solid voxel id.",
		FirstSample.VoxelId, 10);
	TestTrue("Middle sample should remain empty.",
		MiddleSample.IsEmptyVoxel());
	TestEqual("Last sample should match last configured solid voxel id.",
		LastSample.VoxelId, 20);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SeparatePlannersAfterEdit_SampleEditedAndUntouchedPositions,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GetBorderVoxel.SeparatePlannersAfterEditSampleEditedAndUntouchedPositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GetBorderVoxel_SeparatePlannersAfterEdit_SampleEditedAndUntouchedPositions::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	FVoxelMesh VoxelMeshContainer;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector EditedPosition(1, 0, 0);
	const FIntVector UntouchedPosition(0, 1, 1);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(EditedPosition), FRLEVoxel{1, FVoxel(64)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher EditedPlanner;
	FBorderEventMesher UntouchedPlanner;
	const FVoxel EditedSample = RLEChunk->GetBorderVoxel(EditedPlanner, EditedPosition);
	const FVoxel UntouchedSample = RLEChunk->GetBorderVoxel(UntouchedPlanner, UntouchedPosition);

	// Assert
	TestEqual("Edited position sampled by fresh planner should return edited voxel id.",
		EditedSample.VoxelId, 64);
	TestTrue("Untouched position sampled by separate planner should remain empty.",
		UntouchedSample.IsEmptyVoxel());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoBordersThenAllBordersOnEditedChunk_OnlyAllBordersProducesSections,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.NoBordersThenAllBordersOnEditedChunkOnlyAllBordersProducesSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_NoBordersThenAllBordersOnEditedChunk_OnlyAllBordersProducesSections::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> EmptyGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(FVoxel::EMPTY_VOXEL));
	RLEChunk->CompressVoxelModel(EmptyGrid);
	FVoxelMesh MeshNoBorders;
	FVoxelMesh MeshAllBorders;
	FChunkBorderContext NoBordersContext;
	NoBordersContext.BorderVisualization = EBorderVisualizationOption::None;
	FChunkBorderContext AllBordersContext;
	AllBordersContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> FirstCallEdits;
	TArray<FRLEVoxelEdit> SecondCallEdits;
	FirstCallEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(0, 0, 0), FRLEVoxel{1, FVoxel(90)}});

	// Act
	RLEChunk->GenerateMesh(MeshNoBorders, NoBordersContext, FirstCallEdits);
	RLEChunk->GenerateMesh(MeshAllBorders, AllBordersContext, SecondCallEdits);

	// Assert
	TestEqual("No border visualization should keep mesh sections at zero even with edit.",
		MeshNoBorders.VoxelTable.Num(), 0);
	TestTrue("All borders visualization should produce mesh sections from persisted edit.",
		MeshAllBorders.VoxelTable.Num() > 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditOneVoxel_PreservesNeighborVoxelValue,
	"RDM.RDMMeshersTests.RLEVirtualChunk.GenerateMesh.EditOneVoxelPreservesNeighborVoxelValue",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLEVirtualChunk_GenerateMesh_EditOneVoxel_PreservesNeighborVoxelValue::RunTest(
	const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = CreateVoxelData(2);
	URLEVirtualChunk* RLEChunk = CreateRLEChunk(BaseVoxelDataDummy);
	TArray<FVoxel> SolidGrid = CreateFilledVoxelGrid(BaseVoxelDataDummy, FVoxel(5));
	RLEChunk->CompressVoxelModel(SolidGrid);
	FVoxelMesh VoxelMeshContainer;
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;
	TArray<FRLEVoxelEdit> VoxelEdits;
	const FIntVector EditedPosition(0, 0, 1);
	const FIntVector NeighborPosition(0, 1, 1);
	VoxelEdits.Add(FRLEVoxelEdit{BaseVoxelDataDummy->CalculateVoxelIndex(EditedPosition), FRLEVoxel{1, FVoxel(8)}});

	// Act
	RLEChunk->GenerateMesh(VoxelMeshContainer, BorderContext, VoxelEdits);
	FBorderEventMesher BorderEventMesher;
	const FVoxel EditedSample = RLEChunk->GetBorderVoxel(BorderEventMesher, EditedPosition);
	const FVoxel NeighborSample = RLEChunk->GetBorderVoxel(BorderEventMesher, NeighborPosition);

	// Assert
	TestEqual("Edited position should return updated voxel id.",
		EditedSample.VoxelId, 8);
	TestEqual("Neighbor position should keep original solid voxel id.",
		NeighborSample.VoxelId, 5);

	return true;
}

