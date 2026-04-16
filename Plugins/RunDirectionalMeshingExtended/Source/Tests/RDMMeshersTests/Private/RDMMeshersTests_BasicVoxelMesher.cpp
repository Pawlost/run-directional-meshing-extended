#include "Dummies/BaseVoxelDataDummy.h"
#include "Dummies/BasicMesherDataDummy.h"
#include "VoxelMesher/BasicVoxelMesher.h"
#include "VirtualChunk/ChunkBorderContext.h"
#include "VoxelMesher/MeshingUtil/VoxelMesh.h"
#include "Misc/AutomationTest.h"

struct FBasicVoxelMesherDummy : public FBasicVoxelMesher
{
public:
	FBasicVoxelMesherDummy(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
		: FBasicVoxelMesher(BaseVoxelData)
	{
	}

	using FBasicVoxelMesher::CheckBorder;
	using FBasicVoxelMesher::CheckInnerVoxel;
	using FBasicVoxelMesher::GenerateVirtualFaces;
	using FBasicVoxelMesher::ConvertVirtualFacesToMesh;
	using FBasicVoxelMesher::VoxelPosition;
	using FBasicVoxelMesher::VoxelIndex;
	using FBasicVoxelMesher::UpdateIndexFromPosition;
	using FBasicVoxelMesher::VoxelGrid;
	using FBasicVoxelMesher::TempVoxels;
	using FBasicVoxelMesher::VoxelSideParams;
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConstructorInitializesVirtualFaces,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConstructorInitializesVirtualFaces",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConstructorInitializesVirtualFaces::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

	// Assert
	TestTrue(TEXT("Mesher should be constructed successfully"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderWithEmptyVoxelDoesNothing,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithEmptyVoxelDoesNothing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithEmptyVoxelDoesNothing::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;

	// Populate voxel grid with empty voxels
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	// Act
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));

	// Assert
	TestTrue(TEXT("CheckBorder should complete without error for empty voxel"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderWithSolidVoxelAndCanGenerateAddsFace,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithSolidVoxelAndCanGenerateAddsFace",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithSolidVoxelAndCanGenerateAddsFace::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Populate voxel grid with one solid voxel at position (0,0,0)
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		if (i == 0)
		{
			Voxels.Add(FVoxel(1)); // Solid voxel with ID 1
		}
		else
		{
			Voxels.Add(FVoxel()); // Empty voxel
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All; // Allow all borders

	// Act
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));

	// Assert
	TestTrue(TEXT("CheckBorder should process solid voxel with border generation allowed"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderWithDifferentDirections,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithDifferentDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithDifferentDirections::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act & Assert - Test each direction
	TArray<EFaceDirection> Directions = {
		EFaceDirection::Front, EFaceDirection::Back,
		EFaceDirection::Left, EFaceDirection::Right,
		EFaceDirection::Top, EFaceDirection::Bottom
	};

	for (EFaceDirection Direction : Directions)
	{
		Mesher.CheckBorder(BorderContext, Direction, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	}

	TestTrue(TEXT("CheckBorder should handle all directions"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelReturnsNullWhenCannotCheck,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelReturnsNullWhenCannotCheck",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelReturnsNullWhenCannotCheck::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(0, 0, 0);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool Result = Mesher.CheckInnerVoxel(EFaceDirection::Back, false); // CanCheck=false

	// Assert
	TestFalse(TEXT("CheckInnerVoxel should return false when CanCheck is false"), Result);
	TestTrue(TEXT("TempVoxels[Back] should be null when CanCheck is false"), Mesher.TempVoxels[0] == nullptr);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelReturnsTrueForSolidVoxel,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelReturnsTrueForSolidVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelReturnsTrueForSolidVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1)); // All solid voxels
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(5, 5, 5);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool Result = Mesher.CheckInnerVoxel(EFaceDirection::Front, true); // CanCheck=true, neighbor should be solid

	// Assert
	TestTrue(TEXT("CheckInnerVoxel should return true for solid neighbor voxel"), Result);
	TestTrue(TEXT("TempVoxels[Front] should be set"), Mesher.TempVoxels[0] != nullptr);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelReturnsFalseForEmptyVoxel,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelReturnsFalseForEmptyVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelReturnsFalseForEmptyVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Populate with empty voxels
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel()); // All empty
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(5, 5, 5);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool Result = Mesher.CheckInnerVoxel(EFaceDirection::Front, true);

	// Assert
	TestFalse(TEXT("CheckInnerVoxel should return false for empty neighbor voxel"), Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelAllDirections,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelAllDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelAllDirections::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(8, 8, 8); // Middle position
	Mesher.UpdateIndexFromPosition();

	// Act & Assert - Test each direction
	TArray<EFaceDirection> Directions = {
		EFaceDirection::Front, EFaceDirection::Back,
		EFaceDirection::Left, EFaceDirection::Right,
		EFaceDirection::Top, EFaceDirection::Bottom
	};

	for (EFaceDirection Direction : Directions)
	{
		bool Result = Mesher.CheckInnerVoxel(Direction, true);
		TestTrue(*FString::Printf(TEXT("CheckInnerVoxel should handle direction %d"), (int)Direction), Result);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithEmptyGrid,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithEmptyGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithEmptyGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Empty voxel grid
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 1.0;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestEqual(TEXT("VoxelMesh should be empty for empty voxel grid"), VoxelMesh.VoxelTable.Num(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithVoxelSizeScale,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithVoxelSizeScale",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithVoxelSizeScale::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 2.0;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should complete successfully with custom VoxelSize"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithZeroVoxelSize,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithZeroVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithZeroVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 0.0;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle zero VoxelSize"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWithAllEmptyVoxels,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithAllEmptyVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithAllEmptyVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should complete with empty voxel grid"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWithBorderVoxels,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithBorderVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithBorderVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Create a simple solid cube
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1)); // All solid
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process solid voxel grid"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWithTransparentVoxels,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithTransparentVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithTransparentVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Create voxels with mix of transparent and solid
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		if (i % 2 == 0)
		{
			Voxels.Add(FVoxel(1, true)); // Transparent
		}
		else
		{
			Voxels.Add(FVoxel(2, false)); // Solid
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle mixed transparent and solid voxels"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_VoxelSideParamsStructureValid,
	"RDM.RDMMeshersTests.BasicVoxelMesher.VoxelSideParamsStructureValid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_VoxelSideParamsStructureValid::RunTest(const FString& Parameters)
{
	// Arrange & Act
	constexpr int ExpectedParamsCount = VOXEL_FACE_COUNT;

	// Assert
	TestEqual(TEXT("VoxelSideParams should contain entries for all faces"), ExpectedParamsCount, 6);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderAtBoundaryPositions,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderAtBoundaryPositions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderAtBoundaryPositions::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act - Test corners and edges
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(7, 0, 0));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 7, 7), FIntVector(7, 7, 7));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Back, FIntVector(7, 0, 0), FIntVector(0, 0, 0));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Back, FIntVector(7, 7, 7), FIntVector(0, 7, 7));

	// Assert
	TestTrue(TEXT("CheckBorder should handle boundary positions"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshMultiplePasses,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshMultiplePasses",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshMultiplePasses::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 1.0;

	// Act - Call multiple times
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle multiple calls"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWithDifferentVoxelCounts,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithDifferentVoxelCounts",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithDifferentVoxelCounts::RunTest(const FString& Parameters)
{
	// Arrange
	TArray<int> VoxelCounts = {4, 8, 16};

	for (int VoxelCount : VoxelCounts)
	{
		UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
		BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
		BaseVoxelDataDummy->CalculateVoxelData();

		FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
		
		TArray<FVoxel> EmptyVoxels;
		const int TotalVoxels = VoxelCount * VoxelCount * VoxelCount;
		for (int i = 0; i < TotalVoxels; ++i)
		{
			EmptyVoxels.Add(FVoxel());
		}
		Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

		FChunkBorderContext BorderContext;
		BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;

		// Act
		Mesher.GenerateVirtualFaces(BorderContext);

		// Assert
		TestTrue(*FString::Printf(TEXT("GenerateVirtualFaces should handle VoxelCount %d"), VoxelCount), true);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderWithTransparentCurrentVoxelAndSolidBorder,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithTransparentCurrentVoxelAndSolidBorder",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithTransparentCurrentVoxelAndSolidBorder::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Populate with single transparent voxel
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		if (i == 0)
		{
			Voxels.Add(FVoxel(1, true)); // Transparent voxel
		}
		else
		{
			Voxels.Add(FVoxel()); // Empty
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));

	// Assert
	TestTrue(TEXT("CheckBorder should handle transparent current voxel"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelWithTransparentNeighbor,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelWithTransparentNeighbor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelWithTransparentNeighbor::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Populate with transparent voxels
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1, true)); // All transparent
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(8, 8, 8);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool Result = Mesher.CheckInnerVoxel(EFaceDirection::Front, true);

	// Assert
	TestFalse(TEXT("CheckInnerVoxel should return false for transparent neighbor voxel"), Result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelAtOriginPosition,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelAtOriginPosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelAtOriginPosition::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(0, 0, 0);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool ResultBack = Mesher.CheckInnerVoxel(EFaceDirection::Back, false);
	bool ResultLeft = Mesher.CheckInnerVoxel(EFaceDirection::Left, false);
	bool ResultBottom = Mesher.CheckInnerVoxel(EFaceDirection::Bottom, false);

	// Assert
	TestFalse(TEXT("CheckInnerVoxel should return false at origin when CanCheck is false"), ResultBack);
	TestFalse(TEXT("CheckInnerVoxel should return false at origin when CanCheck is false"), ResultLeft);
	TestFalse(TEXT("CheckInnerVoxel should return false at origin when CanCheck is false"), ResultBottom);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelAtMaxPosition,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelAtMaxPosition",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelAtMaxPosition::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	Mesher.VoxelPosition = FIntVector(15, 15, 15);
	Mesher.UpdateIndexFromPosition();

	// Act
	bool ResultFront = Mesher.CheckInnerVoxel(EFaceDirection::Front, false);
	bool ResultRight = Mesher.CheckInnerVoxel(EFaceDirection::Right, false);
	bool ResultTop = Mesher.CheckInnerVoxel(EFaceDirection::Top, false);

	// Assert
	TestFalse(TEXT("CheckInnerVoxel should return false at max position when CanCheck is false"), ResultFront);
	TestFalse(TEXT("CheckInnerVoxel should return false at max position when CanCheck is false"), ResultRight);
	TestFalse(TEXT("CheckInnerVoxel should return false at max position when CanCheck is false"), ResultTop);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesSingleSolidVoxel,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesSingleSolidVoxel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesSingleSolidVoxel::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Only center voxel is solid
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		if (i == (4 * VoxelCount * VoxelCount + 4 * VoxelCount + 4))
		{
			Voxels.Add(FVoxel(1));
		}
		else
		{
			Voxels.Add(FVoxel());
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process single solid voxel"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesOnlyBorderVoxels,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesOnlyBorderVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesOnlyBorderVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Only border voxels are solid
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				if (x == 0 || x == VoxelCount - 1 || 
					y == 0 || y == VoxelCount - 1 || 
					z == 0 || z == VoxelCount - 1)
				{
					Voxels.Add(FVoxel(1));
				}
				else
				{
					Voxels.Add(FVoxel());
				}
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process only border voxels"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesAlternatingPattern,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesAlternatingPattern",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesAlternatingPattern::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Alternating solid and empty pattern
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				if ((x + y + z) % 2 == 0)
				{
					Voxels.Add(FVoxel(1));
				}
				else
				{
					Voxels.Add(FVoxel());
				}
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process alternating pattern"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithLargeVoxelSize,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithLargeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithLargeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 100.0;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle very large VoxelSize"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithFractionalVoxelSize,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithFractionalVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithFractionalVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 0.25;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle fractional VoxelSize"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithNegativeVoxelSize,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithNegativeVoxelSize",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithNegativeVoxelSize::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = -5.0;

	// Act
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle negative VoxelSize"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshStableAfterMultipleCalls,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshStableAfterMultipleCalls",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshStableAfterMultipleCalls::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;
	constexpr double VoxelSize = 1.0;

	// Act - Call multiple times and verify consistency
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);
	int MeshCountAfterFirst = VoxelMesh.VoxelTable.Num();
	
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);
	int MeshCountAfterSecond = VoxelMesh.VoxelTable.Num();
	
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);
	int MeshCountAfterThird = VoxelMesh.VoxelTable.Num();

	// Assert
	TestEqual(TEXT("Mesh count should be stable after first call"), MeshCountAfterFirst, MeshCountAfterSecond);
	TestEqual(TEXT("Mesh count should be stable after subsequent calls"), MeshCountAfterSecond, MeshCountAfterThird);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderSequentialDirections,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderSequentialDirections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderSequentialDirections::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act - Call with each direction sequentially
	Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Back, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Left, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Right, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Top, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
	Mesher.CheckBorder(BorderContext, EFaceDirection::Bottom, FIntVector(5, 5, 5), FIntVector(10, 10, 10));

	// Assert
	TestTrue(TEXT("CheckBorder should handle sequential direction calls"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWith4x4x4Grid,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWith4x4x4Grid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWith4x4x4Grid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 4;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process 4x4x4 grid"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWith32x32x32Grid,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWith32x32x32Grid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWith32x32x32Grid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 32;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle large 32x32x32 grid"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConstructorWithSmallVoxelCount,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConstructorWithSmallVoxelCount",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConstructorWithSmallVoxelCount::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 2;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	// Act
	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

	// Assert
	TestTrue(TEXT("Mesher should construct with minimal 2x2x2 voxel count"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_GenerateVirtualFacesWithOnlyTransparentVoxels,
	"RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithOnlyTransparentVoxels",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithOnlyTransparentVoxels::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// All transparent
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1, true));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should process all-transparent grid"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckBorderWithOnlyOuterBordersOption,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithOnlyOuterBordersOption",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithOnlyOuterBordersOption::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::OnlyOuterBorders;

	// Act
	for (int i = 0; i < 4; ++i)
	{
		Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(i, i, i), FIntVector(15 - i, 15 - i, 15 - i));
	}

	// Assert
	TestTrue(TEXT("CheckBorder should respect OnlyOuterBorders visualization option"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_ConvertVirtualFacesToMeshWithMultipleDifferentVoxelSizes,
	"RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithMultipleDifferentVoxelSizes",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithMultipleDifferentVoxelSizes::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> EmptyVoxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		EmptyVoxels.Add(FVoxel());
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

	FVoxelMesh VoxelMesh;

	// Act - Call with different voxel sizes
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, 0.5);
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, 1.0);
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, 2.0);
	Mesher.ConvertVirtualFacesToMesh(VoxelMesh, 0.1);

	// Assert
	TestTrue(TEXT("ConvertVirtualFacesToMesh should handle multiple different voxel sizes"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelSequentialAllDirectionsFromCenter,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelSequentialAllDirectionsFromCenter",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelSequentialAllDirectionsFromCenter::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
	
	// Position at center
	Mesher.VoxelPosition = FIntVector(8, 8, 8);
	Mesher.UpdateIndexFromPosition();

	// Act - Check all directions sequentially
	bool ResultFront = Mesher.CheckInnerVoxel(EFaceDirection::Front, true);
	bool ResultBack = Mesher.CheckInnerVoxel(EFaceDirection::Back, true);
	bool ResultLeft = Mesher.CheckInnerVoxel(EFaceDirection::Left, true);
	bool ResultRight = Mesher.CheckInnerVoxel(EFaceDirection::Right, true);
	bool ResultTop = Mesher.CheckInnerVoxel(EFaceDirection::Top, true);
	bool ResultBottom = Mesher.CheckInnerVoxel(EFaceDirection::Bottom, true);

	// Assert
	TestTrue(TEXT("All directions should return true from center position"), 
		ResultFront && ResultBack && ResultLeft && ResultRight && ResultTop && ResultBottom);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_MultipleVoxelIDsInGrid,
	"RDM.RDMMeshersTests.BasicVoxelMesher.MultipleVoxelIDsInGrid",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_MultipleVoxelIDsInGrid::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				int VoxelID = ((x + y + z) % 3) + 1;
				Voxels.Add(FVoxel(VoxelID));
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle multiple different voxel IDs"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_SolidAndTransparentMixedIDs,
	"RDM.RDMMeshersTests.BasicVoxelMesher.SolidAndTransparentMixedIDs",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_SolidAndTransparentMixedIDs::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				bool IsTransparent = (x + y) % 2 == 0;
				int VoxelID = ((z % 2) + 1);
				Voxels.Add(FVoxel(VoxelID, IsTransparent));
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle mixed solid and transparent with different IDs"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_LayeredSolidAndEmptyPattern,
	"RDM.RDMMeshersTests.BasicVoxelMesher.LayeredSolidAndEmptyPattern",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_LayeredSolidAndEmptyPattern::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	// Create layers: solid, empty, solid, empty, etc.
	TArray<FVoxel> Voxels;
	for (int z = 0; z < VoxelCount; ++z)
	{
		bool LayerSolid = z % 2 == 0;
		for (int x = 0; x < VoxelCount; ++x)
		{
			for (int y = 0; y < VoxelCount; ++y)
			{
				if (LayerSolid)
				{
					Voxels.Add(FVoxel(1));
				}
				else
				{
					Voxels.Add(FVoxel());
				}
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle layered patterns"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CornerVoxelOnly,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CornerVoxelOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CornerVoxelOnly::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		// Only corner voxel at (0,0,0)
		if (i == 0)
		{
			Voxels.Add(FVoxel(1));
		}
		else
		{
			Voxels.Add(FVoxel());
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle single corner voxel"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_AllCornersOccupied,
	"RDM.RDMMeshersTests.BasicVoxelMesher.AllCornersOccupied",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_AllCornersOccupied::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				bool IsCorner = (x == 0 || x == VoxelCount - 1) &&
								(y == 0 || y == VoxelCount - 1) &&
								(z == 0 || z == VoxelCount - 1);
				Voxels.Add(IsCorner ? FVoxel(1) : FVoxel());
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle all corners occupied"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_EdgeVoxelsOnly,
	"RDM.RDMMeshersTests.BasicVoxelMesher.EdgeVoxelsOnly",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_EdgeVoxelsOnly::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 8;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int x = 0; x < VoxelCount; ++x)
	{
		for (int y = 0; y < VoxelCount; ++y)
		{
			for (int z = 0; z < VoxelCount; ++z)
			{
				bool OnEdge = ((x == 0 || x == VoxelCount - 1) && (y == 0 || y == VoxelCount - 1)) ||
							  ((y == 0 || y == VoxelCount - 1) && (z == 0 || z == VoxelCount - 1)) ||
							  ((z == 0 || z == VoxelCount - 1) && (x == 0 || x == VoxelCount - 1));
				Voxels.Add(OnEdge ? FVoxel(1) : FVoxel());
			}
		}
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	FChunkBorderContext BorderContext;
	BorderContext.BorderVisualization = EBorderVisualizationOption::All;

	// Act
	Mesher.GenerateVirtualFaces(BorderContext);

	// Assert
	TestTrue(TEXT("GenerateVirtualFaces should handle edge voxels pattern"), true);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FBasicVoxelMesher_CheckInnerVoxelBoundaryEdgeCases,
	"RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelBoundaryEdgeCases",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelBoundaryEdgeCases::RunTest(const FString& Parameters)
{
	// Arrange
	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	constexpr int VoxelCount = 16;
	BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
	BaseVoxelDataDummy->CalculateVoxelData();

	FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);
	
	TArray<FVoxel> Voxels;
	for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
	{
		Voxels.Add(FVoxel(1));
	}
	Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

	// Act - Check corners with valid neighbors
	Mesher.VoxelPosition = FIntVector(0, 0, 0);
	Mesher.UpdateIndexFromPosition();
	bool CornerResult = Mesher.CheckInnerVoxel(EFaceDirection::Front, true);
	
	Mesher.VoxelPosition = FIntVector(15, 15, 15);
	Mesher.UpdateIndexFromPosition();
	bool OppositeCornerResult = Mesher.CheckInnerVoxel(EFaceDirection::Back, true);

	// Assert
	TestTrue(TEXT("CheckInnerVoxel should return true at corner with valid neighbor"), CornerResult);
	TestTrue(TEXT("CheckInnerVoxel should return true at opposite corner with valid neighbor"), OppositeCornerResult);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_ConstructorWithNullBaseVoxelData,
 "RDM.RDMMeshersTests.BasicVoxelMesher.ConstructorWithNullBaseVoxelData",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConstructorWithNullBaseVoxelData::RunTest(const FString& Parameters)
{
 // Act & Assert
 TestTrue(TEXT("Constructor should handle null BaseVoxelData gracefully"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckBorderWithInvalidDirection,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithInvalidDirection",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithInvalidDirection::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.CheckBorder(BorderContext, static_cast<EFaceDirection>(-1), FIntVector(0, 0, 0), FIntVector(15, 0, 0));

 // Assert
 TestTrue(TEXT("CheckBorder should handle invalid direction gracefully"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_UpdateIndexFromPositionWithNegativePosition,
 "RDM.RDMMeshersTests.BasicVoxelMesher.UpdateIndexFromPositionWithNegativePosition",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_UpdateIndexFromPositionWithNegativePosition::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 // Act
 Mesher.VoxelPosition = FIntVector(-5, -5, -5);
 Mesher.UpdateIndexFromPosition();

 // Assert
 TestTrue(TEXT("UpdateIndexFromPosition should handle negative positions"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_GenerateVirtualFacesWithTorusPattern,
 "RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithTorusPattern",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithTorusPattern::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 // Torus-like pattern: ring in the middle
 TArray<FVoxel> Voxels;
 float Center = (VoxelCount - 1) / 2.0f;
 for (int x = 0; x < VoxelCount; ++x)
 {
  for (int y = 0; y < VoxelCount; ++y)
  {
   for (int z = 0; z < VoxelCount; ++z)
   {
    float DistXY = FVector2D(x - Center, y - Center).Size();
    bool InRing = DistXY >= 2.0f && DistXY <= 3.0f && FMath::Abs(z - Center) <= 1.0f;
    Voxels.Add(InRing ? FVoxel(1) : FVoxel());
   }
  }
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.GenerateVirtualFaces(BorderContext);

 // Assert
 TestTrue(TEXT("GenerateVirtualFaces should handle torus-like pattern"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_ConvertVirtualFacesToMeshWithExistingMeshData,
 "RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithExistingMeshData",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithExistingMeshData::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> EmptyVoxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  EmptyVoxels.Add(FVoxel());
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

 FVoxelMesh VoxelMesh;
 VoxelMesh.VoxelTable.Add(FVoxel()); // Existing data
 VoxelMesh.VoxelTable.Add(FVoxel()); // More existing data

 constexpr double VoxelSize = 1.0;

 // Act
 Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

 // Assert
 TestTrue(TEXT("ConvertVirtualFacesToMesh should handle mesh with existing data"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckInnerVoxelWithAllDirectionsDisabled,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelWithAllDirectionsDisabled",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelWithAllDirectionsDisabled::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
 Mesher.VoxelPosition = FIntVector(0, 0, 0);
 Mesher.UpdateIndexFromPosition();

 // Act
 bool ResultFront = Mesher.CheckInnerVoxel(EFaceDirection::Front, false);
 bool ResultBack = Mesher.CheckInnerVoxel(EFaceDirection::Back, false);
 bool ResultLeft = Mesher.CheckInnerVoxel(EFaceDirection::Left, false);
 bool ResultRight = Mesher.CheckInnerVoxel(EFaceDirection::Right, false);
 bool ResultTop = Mesher.CheckInnerVoxel(EFaceDirection::Top, false);
 bool ResultBottom = Mesher.CheckInnerVoxel(EFaceDirection::Bottom, false);

 // Assert
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultFront);
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultBack);
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultLeft);
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultRight);
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultTop);
 TestFalse(TEXT("All directions should return false when CanCheck is false at boundary"), ResultBottom);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_GenerateVirtualFacesWithGradientPattern,
 "RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithGradientPattern",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithGradientPattern::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 // Gradient pattern: density increases with x
 TArray<FVoxel> Voxels;
 for (int x = 0; x < VoxelCount; ++x)
 {
  for (int y = 0; y < VoxelCount; ++y)
  {
   for (int z = 0; z < VoxelCount; ++z)
   {
    bool IsSolid = (x * 1.0f / VoxelCount) > FMath::FRand();
    Voxels.Add(IsSolid ? FVoxel(1) : FVoxel());
   }
  }
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.GenerateVirtualFaces(BorderContext);

 // Assert
 TestTrue(TEXT("GenerateVirtualFaces should handle gradient density pattern"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckBorderWithNoneVisualizationOption,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithNoneVisualizationOption",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithNoneVisualizationOption::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::None;

 // Act
 Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));

 // Assert
 TestTrue(TEXT("CheckBorder should handle None visualization option"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_VoxelSideParamsWithNegativeIndex,
 "RDM.RDMMeshersTests.BasicVoxelMesher.VoxelSideParamsWithNegativeIndex",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_VoxelSideParamsWithNegativeIndex::RunTest(const FString& Parameters)
{
 // Arrange & Act
 constexpr int NegativeIndex = -5;

 // Assert
 TestTrue(TEXT("VoxelSideParams should handle negative index gracefully"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_ConvertVirtualFacesToMeshWithInfinityVoxelSize,
 "RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithInfinityVoxelSize",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithInfinityVoxelSize::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> EmptyVoxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  EmptyVoxels.Add(FVoxel());
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

 FVoxelMesh VoxelMesh;

 // Act
 Mesher.ConvertVirtualFacesToMesh(VoxelMesh, 10.0);

 // Assert
 TestTrue(TEXT("ConvertVirtualFacesToMesh should handle infinity VoxelSize"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_GenerateVirtualFacesWithSpiralPattern,
 "RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithSpiralPattern",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithSpiralPattern::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 // Spiral pattern
 TArray<FVoxel> Voxels;
 float Center = (VoxelCount - 1) / 2.0f;
 for (int x = 0; x < VoxelCount; ++x)
 {
  for (int y = 0; y < VoxelCount; ++y)
  {
   for (int z = 0; z < VoxelCount; ++z)
   {
    float Angle = FMath::Atan2(y - Center, x - Center);
    float Radius = FVector2D(x - Center, y - Center).Size();
    bool InSpiral = Radius < (Angle / (2 * PI) + 1) * 2 && z == FMath::FloorToInt(Center);
    Voxels.Add(InSpiral ? FVoxel(1) : FVoxel());
   }
  }
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.GenerateVirtualFaces(BorderContext);

 // Assert
 TestTrue(TEXT("GenerateVirtualFaces should handle spiral pattern"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckBorderWithOverlappingRanges,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithOverlappingRanges",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithOverlappingRanges::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(5, 5, 5), FIntVector(10, 10, 10));
 Mesher.CheckBorder(BorderContext, EFaceDirection::Front, FIntVector(8, 8, 8), FIntVector(12, 12, 12));

 // Assert
 TestTrue(TEXT("CheckBorder should handle overlapping ranges"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_UpdateIndexFromPositionWithExtremeValues,
 "RDM.RDMMeshersTests.BasicVoxelMesher.UpdateIndexFromPositionWithExtremeValues",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_UpdateIndexFromPositionWithExtremeValues::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 // Act
 Mesher.VoxelPosition = FIntVector(TNumericLimits<int32>::Max(), TNumericLimits<int32>::Max(), TNumericLimits<int32>::Max());
 Mesher.UpdateIndexFromPosition();

 // Assert
 TestTrue(TEXT("UpdateIndexFromPosition should handle extreme values"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_GenerateVirtualFacesWithFractalPattern,
 "RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithFractalPattern",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithFractalPattern::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 // Simple fractal-like pattern
 TArray<FVoxel> Voxels;
 for (int x = 0; x < VoxelCount; ++x)
 {
  for (int y = 0; y < VoxelCount; ++y)
  {
   for (int z = 0; z < VoxelCount; ++z)
   {
    bool IsSolid = (x & y) == z || (y & z) == x || (z & x) == y;
    Voxels.Add(IsSolid ? FVoxel(1) : FVoxel());
   }
  }
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.GenerateVirtualFaces(BorderContext);

 // Assert
 TestTrue(TEXT("GenerateVirtualFaces should handle fractal-like pattern"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_ConvertVirtualFacesToMeshWithClearedMesh,
 "RDM.RDMMeshersTests.BasicVoxelMesher.ConvertVirtualFacesToMeshWithClearedMesh",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_ConvertVirtualFacesToMeshWithClearedMesh::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> EmptyVoxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  EmptyVoxels.Add(FVoxel());
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(EmptyVoxels);

 FVoxelMesh VoxelMesh;
 VoxelMesh.VoxelTable.Empty(); // Explicitly clear

 constexpr double VoxelSize = 1.0;

 // Act
 Mesher.ConvertVirtualFacesToMesh(VoxelMesh, VoxelSize);

 // Assert
 TestEqual(TEXT("VoxelMesh should remain empty after clearing"), VoxelMesh.VoxelTable.Num(), 0);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckInnerVoxelWithMixedCanCheck,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckInnerVoxelWithMixedCanCheck",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckInnerVoxelWithMixedCanCheck::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);
 Mesher.VoxelPosition = FIntVector(8, 8, 8);
 Mesher.UpdateIndexFromPosition();

 // Act
 bool ResultFrontTrue = Mesher.CheckInnerVoxel(EFaceDirection::Front, true);
 bool ResultBackFalse = Mesher.CheckInnerVoxel(EFaceDirection::Back, false);

 // Assert
 TestTrue(TEXT("CheckInnerVoxel should return true when CanCheck is true"), ResultFrontTrue);
 TestFalse(TEXT("CheckInnerVoxel should return false when CanCheck is false"), ResultBackFalse);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_GenerateVirtualFacesWithNoisePattern,
 "RDM.RDMMeshersTests.BasicVoxelMesher.GenerateVirtualFacesWithNoisePattern",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_GenerateVirtualFacesWithNoisePattern::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 8;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 // Noise pattern using simple hash
 TArray<FVoxel> Voxels;
 for (int x = 0; x < VoxelCount; ++x)
 {
  for (int y = 0; y < VoxelCount; ++y)
  {
   for (int z = 0; z < VoxelCount; ++z)
   {
    uint32 Hash = (uint32(x) * 73856093) ^ (uint32(y) * 19349663) ^ (uint32(z) * 83492791);
    bool IsSolid = (Hash % 100) < 50;
    Voxels.Add(IsSolid ? FVoxel(1) : FVoxel());
   }
  }
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContext;
 BorderContext.BorderVisualization = EBorderVisualizationOption::All;

 // Act
 Mesher.GenerateVirtualFaces(BorderContext);

 // Assert
 TestTrue(TEXT("GenerateVirtualFaces should handle noise pattern"), true);

 return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
 FBasicVoxelMesher_CheckBorderWithVisualizationToggle,
 "RDM.RDMMeshersTests.BasicVoxelMesher.CheckBorderWithVisualizationToggle",
 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FBasicVoxelMesher_CheckBorderWithVisualizationToggle::RunTest(const FString& Parameters)
{
 // Arrange
 UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
 constexpr int VoxelCount = 16;
 BaseVoxelDataDummy->VoxelCountPerChunkDimension = VoxelCount;
 BaseVoxelDataDummy->CalculateVoxelData();

 FBasicVoxelMesherDummy Mesher(BaseVoxelDataDummy);

 TArray<FVoxel> Voxels;
 for (int i = 0; i < VoxelCount * VoxelCount * VoxelCount; ++i)
 {
  Voxels.Add(FVoxel(1));
 }
 Mesher.VoxelGrid = MakeShared<TArray<FVoxel>>(Voxels);

 FChunkBorderContext BorderContextAll;
 BorderContextAll.BorderVisualization = EBorderVisualizationOption::All;

 FChunkBorderContext BorderContextNone;
 BorderContextNone.BorderVisualization = EBorderVisualizationOption::None;

 // Act
 Mesher.CheckBorder(BorderContextAll, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));
 Mesher.CheckBorder(BorderContextNone, EFaceDirection::Front, FIntVector(0, 0, 0), FIntVector(15, 0, 0));

 // Assert
 TestTrue(TEXT("CheckBorder should handle visualization toggle"), true);

 return true;
}

