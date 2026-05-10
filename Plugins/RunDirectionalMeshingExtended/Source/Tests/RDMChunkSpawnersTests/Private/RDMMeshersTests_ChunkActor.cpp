#include "Misc/AutomationTest.h"

#define private public
#define protected public
#include "Chunk/ChunkActor.h"
#undef protected
#undef private

#include "Dummies/ChunkActorDummy.h"
#include "Engine/DataTable.h"
#include "HAL/PlatformProcess.h"
#include "Single/SingleVoxelGenerator.h"
#include "VirtualChunk/BasicVirtualChunk.h"
#include "VoxelMesher/MeshingUtil/RLEVoxelEdit.h"

namespace
{
	static UDataTable* CreateVoxelTable()
	{
		UDataTable* Table = NewObject<UDataTable>();
		Table->RowStruct = FVoxelTableRow::StaticStruct();
		Table->AddRow(TEXT("Stone"), FVoxelTableRow());
		return Table;
	}

	static USingleVoxelGenerator* CreateVoxelGenerator(UObject* Outer)
	{
		USingleVoxelGenerator* Generator = NewObject<USingleVoxelGenerator>(Outer);
		Generator->VoxelTypeHandle.DataTable = CreateVoxelTable();
		Generator->VoxelTypeHandle.RowName = TEXT("Stone");
		return Generator;
	}

	static TSharedPtr<FProcMeshSectionVars> CreateTriangleMeshSection(const uint32 MeshSectionId)
	{
		TSharedPtr<FProcMeshSectionVars> Section = MakeShared<FProcMeshSectionVars>();
		Section->MeshSectionId = MeshSectionId;
		Section->Vertices = {FVector(0, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1)};
		Section->Triangles = {0, 1, 2};
		Section->Normals = {FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0)};
		Section->UV0 = {FVector2D(0, 0), FVector2D(1, 0), FVector2D(0, 1)};
		Section->Tangents = {FProcMeshTangent(0, 1, 0), FProcMeshTangent(0, 1, 0), FProcMeshTangent(0, 1, 0)};
		return Section;
	}

	static void AddMeshSection(UProceduralMeshComponent* ProceduralMeshComponent, const int32 SectionId)
	{
		const TArray<FVector> Vertices = {FVector(0, 0, 0), FVector(0, 1, 0), FVector(0, 0, 1)};
		const TArray<int32> Triangles = {0, 1, 2};
		const TArray<FVector> Normals = {FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0)};
		const TArray<FVector2D> UV0 = {FVector2D(0, 0), FVector2D(1, 0), FVector2D(0, 1)};
		const TArray<FProcMeshTangent> Tangents = {
			FProcMeshTangent(0, 1, 0),
			FProcMeshTangent(0, 1, 0),
			FProcMeshTangent(0, 1, 0)
		};

		ProceduralMeshComponent->CreateMeshSection_LinearColor(
			SectionId,
			Vertices,
			Triangles,
			Normals,
			UV0,
			TArray<FLinearColor>(),
			Tangents,
			true);
	}

	static bool WaitForMeshSections(const UProceduralMeshComponent* ProceduralMeshComponent, const int32 ExpectedSections)
	{
		constexpr int32 MaxRetries = 50;
		constexpr float RetryDelaySeconds = 0.002f;
		for (int32 Retry = 0; Retry < MaxRetries; Retry++)
		{
			if (ProceduralMeshComponent->GetNumSections() == ExpectedSections)
			{
				return true;
			}
			FPlatformProcess::Sleep(RetryDelaySeconds);
		}
		return false;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_ChunkActor_ConstructorCreatesProceduralMeshComponentAsRoot,
	"RDM.RDMChunkSpawnersTests.ChunkActor.Constructor.CreatesProceduralMeshComponentAsRoot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_ChunkActor_ConstructorCreatesProceduralMeshComponentAsRoot::RunTest(const FString& Parameters)
{
	// Arrange
	AChunkActorDummy* ChunkActor = NewObject<AChunkActorDummy>();

	// Act
	const bool bHasProceduralMeshComponent = IsValid(ChunkActor->ProceduralMeshComponent);

	// Assert
	TestTrue("ProceduralMeshComponent should be valid", bHasProceduralMeshComponent);
	TestEqual("RootComponent should be ProceduralMeshComponent", ChunkActor->GetRootComponent(),
	          static_cast<USceneComponent*>(ChunkActor->ProceduralMeshComponent));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_ChunkActor_HasMeshReturnsFalseWhenNoSectionsExist,
	"RDM.RDMChunkSpawnersTests.ChunkActor.HasMesh.ReturnsFalseWhenNoSectionsExist",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_ChunkActor_HasMeshReturnsFalseWhenNoSectionsExist::RunTest(const FString& Parameters)
{
	// Arrange
	AChunkActorDummy* ChunkActor = NewObject<AChunkActorDummy>();

	// Act
	const bool bHasMesh = ChunkActor->HasMesh();

	// Assert
	TestFalse("HasMesh should return false", bHasMesh);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_ChunkActor_HasMeshReturnsTrueWhenSectionExists,
	"RDM.RDMChunkSpawnersTests.ChunkActor.HasMesh.ReturnsTrueWhenSectionExists",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_ChunkActor_HasMeshReturnsTrueWhenSectionExists::RunTest(const FString& Parameters)
{
	// Arrange
	AChunkActorDummy* ChunkActor = NewObject<AChunkActorDummy>();
	AddMeshSection(ChunkActor->ProceduralMeshComponent, 0);

	// Act
	const bool bHasMesh = ChunkActor->HasMesh();

	// Assert
	TestTrue("HasMesh should return true", bHasMesh);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_ChunkActor_ClearMeshRemovesAllSections,
	"RDM.RDMChunkSpawnersTests.ChunkActor.ClearMesh.RemovesAllSections",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_ChunkActor_ClearMeshRemovesAllSections::RunTest(const FString& Parameters)
{
	// Arrange
	AChunkActorDummy* ChunkActor = NewObject<AChunkActorDummy>();
	AddMeshSection(ChunkActor->ProceduralMeshComponent, 0);

	// Act
	ChunkActor->ClearMesh();

	// Assert
	TestFalse("HasMesh should return false after ClearMesh", ChunkActor->HasMesh());
	TestEqual("ProceduralMeshComponent should have no sections", ChunkActor->ProceduralMeshComponent->GetNumSections(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_ChunkActor_EndPlayClearsExistingMesh,
	"RDM.RDMChunkSpawnersTests.ChunkActor.EndPlay.ClearsExistingMesh",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_ChunkActor_EndPlayClearsExistingMesh::RunTest(const FString& Parameters)
{
	// Arrange
	AChunkActorDummy* ChunkActor = NewObject<AChunkActorDummy>();
	AddMeshSection(ChunkActor->ProceduralMeshComponent, 0);

	// Act
	ChunkActor->EndPlay(EEndPlayReason::Type::Destroyed);

	// Assert
	TestFalse("HasMesh should return false after EndPlay", ChunkActor->HasMesh());
	return true;
}