#include "DummyTestClasses.h"
#include "TestData.h"
#include "Misc/AutomationTest.h"
#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestValidVoxelSequenceAfterMeshing,
	"RDM.RDMMeshersTests.RLERunDirectionalVoxelMesher.CheckValidVoxelSequenceAfterMeshing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestValidVoxelSequenceAfterMeshing::RunTest(
	const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<
		UDummyRLERunDirectionalVoxelMesher>();
	auto ChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	auto BorderChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	TArray<FRLEVoxelEdit> VoxelChanges;
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT> BorderChunks;
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TMap<int32, uint32> LocalVoxelTable;
	TMap<int32, uint32> BorderLocalVoxelTable;
	TStaticArray<TSharedPtr<TArray<FRLEVoxel>>, CHUNK_FACE_COUNT> SampledBorderChunks;
	TStaticArray<bool*, CHUNK_FACE_COUNT> IsBorderSampled;

	auto ChunkMeshDataPtr = ChunkMeshData.ToSharedPtr();
	auto BorderChunkMeshDataPtr = BorderChunkMeshData.ToSharedPtr();

	UBaseVoxelDataDummy* BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();

	TArray<FVoxel> VoxelModel;
	auto BasicVoxel = FVoxel(FVoxel::EMPTY_VOXEL + 1);
	BaseVoxelDataDummy->CalculateVoxelData();
	VoxelModel.Init(BasicVoxel, BaseVoxelDataDummy->GetVoxelCountPerChunk());

	VoxelChanges.Add(FRLEVoxelEdit(100, FRLEVoxel(FVoxel::EMPTY_VOXEL)));

	bool IsSampled = false;

	for (int d = 0; d < CHUNK_FACE_COUNT; d++)
	{
		BorderChunks[d] = MakeShared<FBorderChunk>(BaseVoxelDataDummy->GetVoxelCountPerVoxelPlane());
		IsBorderSampled[d] = &IsSampled;
	}

	// Execute
	TStrongObjectPtr<UVoxelModel> RLEVoxelGridPtr = DummyRLERunDirectionalVoxelMesher->CompressVoxelModel(VoxelModel);
	//DummyRLERunDirectionalVoxelMesher->SetVoxelGenerator(BaseVoxelDataDummy);

	/*
	DummyRLERunDirectionalVoxelMesher->GenerateMesh(RLEVoxelGridPtr, 
	VirtualFaces,
	LocalVoxelTable,
	BorderLocalVoxelTable,
	ChunkMeshDataPtr,
	BorderChunkMeshDataPtr,
	VoxelChanges,
	BorderChunks,
	SampledBorderChunks,
	IsBorderSampled,
	true);
	*/
	// Test
	const auto RLEVoxelModelPtr = Cast<URLEVoxelGrid>(RLEVoxelGridPtr.Get());

	int VoxelCount = RLEVoxelModelPtr->RLEVoxelGrid->Num();
	for (int i = 0; i < VoxelCount - 1; i++)
	{
		auto NextVoxel = (*RLEVoxelModelPtr->RLEVoxelGrid)[i + 1];
		auto Voxel = (*RLEVoxelModelPtr->RLEVoxelGrid)[i];
		TestTrue("Each voxel needs to be unique after meshing.", NextVoxel.Voxel != Voxel.Voxel);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestAdvanceAllMeshingEvents,
	"RDM.RDMMeshersTests.RLERunDirectionalVoxelMesher.TestAdvanceAllMeshingEvents",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestAdvanceAllMeshingEvents::RunTest(const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<
		UDummyRLERunDirectionalVoxelMesher>();
	auto ChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	auto BorderChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	TArray<FRLEVoxelEdit> VoxelChanges;
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT> BorderChunks;
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TMap<int32, uint32> LocalVoxelTable;
	TMap<int32, uint32> BorderLocalVoxelTable;
	TStaticArray<TSharedPtr<TArray<FRLEVoxel>>, CHUNK_FACE_COUNT> SampledBorderChunks;
	TStaticArray<bool*, CHUNK_FACE_COUNT> IsBorderSampled;

	auto ChunkMeshDataPtr = ChunkMeshData.ToSharedPtr();
	auto BorderChunkMeshDataPtr = BorderChunkMeshData.ToSharedPtr();

	TObjectPtr<UBaseVoxelDataDummy> BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();

	TArray<FVoxel> VoxelModel;
	auto BasicVoxel = FVoxel(FVoxel::EMPTY_VOXEL + 1);
	BaseVoxelDataDummy->CalculateVoxelData();
	VoxelModel.Init(BasicVoxel, BaseVoxelDataDummy->GetVoxelCountPerChunk());

	VoxelChanges.Add(FRLEVoxelEdit(100, FRLEVoxel(FVoxel::EMPTY_VOXEL)));

	bool IsSampled = false;

	for (int d = 0; d < CHUNK_FACE_COUNT; d++)
	{
		BorderChunks[d] = MakeShared<FBorderChunk>(BaseVoxelDataDummy->GetVoxelCountPerVoxelPlane());
		IsBorderSampled[d] = &IsSampled;
	}

	// Execute
	TStrongObjectPtr<UVoxelModel> RLEVoxelGridPtr = DummyRLERunDirectionalVoxelMesher->CompressVoxelModel(VoxelModel);
	//DummyRLERunDirectionalVoxelMesher->SetVoxelGenerator(BaseVoxelDataDummy);

	/*
	DummyRLERunDirectionalVoxelMesher->AdvanceAllMeshingEvents();
	DummyRLERunDirectionalVoxelMesher->GenerateMesh(RLEVoxelGridPtr, 
	VirtualFaces,
	LocalVoxelTable,
	BorderLocalVoxelTable,
	ChunkMeshDataPtr,
	BorderChunkMeshDataPtr,
	VoxelChanges,
	BorderChunks,
	SampledBorderChunks,
	IsBorderSampled,
	true);
	*/

	// Test
	const auto RLEVoxelModelPtr = Cast<URLEVoxelGrid>(RLEVoxelGridPtr.Get());

	int VoxelCount = RLEVoxelModelPtr->RLEVoxelGrid->Num();
	for (int i = 0; i < VoxelCount - 1; i++)
	{
		auto NextVoxel = (*RLEVoxelModelPtr->RLEVoxelGrid)[i + 1];
		auto Voxel = (*RLEVoxelModelPtr->RLEVoxelGrid)[i];
		TestTrue("Each voxel needs to be unique after meshing.", NextVoxel.Voxel != Voxel.Voxel);
	}

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLERunDirectionalVoxelMesher_Borderless_TestFaceGeneration_NoiseVoxelData,
	"RDM.RDMMeshersTests.RLERunDirectionalVoxelMesher.Borderless.TestFaceGeneration.NoiseVoxelData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLERunDirectionalVoxelMesher_Borderless_TestFaceGeneration_NoiseVoxelData::RunTest(
	const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<UDummyRLERunDirectionalVoxelMesher>();
	TObjectPtr<UBaseVoxelDataDummy> BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	DummyRLERunDirectionalVoxelMesher->RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
	DummyRLERunDirectionalVoxelMesher->RLEVoxelGrid->Append(TestData::NoiseRLEVoxelModel);

	TArray<FRLEVoxelEdit> VoxelChanges;
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT> SideMeshers;
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces;

	BaseVoxelDataDummy->CalculateVoxelData();
	// Execute
	DummyRLERunDirectionalVoxelMesher->SetVoxelGenerator(BaseVoxelDataDummy);
	DummyRLERunDirectionalVoxelMesher->FaceGeneration(VoxelChanges, VirtualFaces, SideFaces, SideMeshers);

	// Test
	

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLERunDirectionalVoxelMesher_Borderless_YTraversal_NoiseVoxelData,
	"RDM.RDMMeshersTests.RLERunDirectionalVoxelMesher.Borderless.YTraversal.NoiseVoxelData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLERunDirectionalVoxelMesher_Borderless_YTraversal_NoiseVoxelData::RunTest(
	const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<UDummyRLERunDirectionalVoxelMesher>();
	TObjectPtr<UBaseVoxelDataDummy> BaseVoxelDataDummy = NewObject<UBaseVoxelDataDummy>();
	DummyRLERunDirectionalVoxelMesher->RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
	DummyRLERunDirectionalVoxelMesher->RLEVoxelGrid->Append(TestData::NoiseRLEVoxelModel);

	TArray<FRLEVoxelEdit> VoxelChanges;
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT> SideMeshers;
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces;
	TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces;
	
	DummyRLERunDirectionalVoxelMesher->SetVoxelGenerator(BaseVoxelDataDummy);
	BaseVoxelDataDummy->CalculateVoxelData();
	
	TStaticArray<UVoxelMesherBase::FIndexParams, CHUNK_FACE_COUNT> BorderIndexParams;
	UVoxelMesherBase::FIndexParams IndexParams;
	
	TArray<FRLEVoxelEdit> VoxelEdits;
	IndexParams.VoxelEdits = &VoxelEdits;
	IndexParams.VirtualFaces = VirtualFaces;
	IndexParams.SideFaces = SideFaces;
	auto VoxelModelSharedPtr = MakeShared<TArray<FRLEVoxel>>(TestData::NoiseRLEVoxelModel);
	
	IndexParams.EditEnabled = false;
	IndexParams.CurrentMeshingEventIndex = 224;
	IndexParams.IndexSequenceBetweenEvents = 32;
	IndexParams.PreviousVoxelRun = &(*VoxelModelSharedPtr)[0];
	IndexParams.NextMeshingEventIndex = 224;
	IndexParams.InitialPosition = FIntVector(0, 0, 6);
	
	IndexParams.MeshingEvents[0].LastEventIndex = 0;
	IndexParams.MeshingEvents[0].VoxelRunIndex = 0;
	IndexParams.MeshingEvents[0].VoxelGridPtr = VoxelModelSharedPtr;
	
	IndexParams.MeshingEvents[1].LastEventIndex = 1024;
	IndexParams.MeshingEvents[1].VoxelRunIndex = 0;
	IndexParams.MeshingEvents[1].VoxelGridPtr = VoxelModelSharedPtr;
	
	IndexParams.MeshingEvents[2].LastEventIndex = 32;
	IndexParams.MeshingEvents[2].VoxelRunIndex = 0;
	IndexParams.MeshingEvents[2].VoxelGridPtr = VoxelModelSharedPtr;
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		SideFaces[f] = MakeShared<TArray<FVirtualVoxelFace>>();
	}
	
	// Execute
	DummyRLERunDirectionalVoxelMesher->TraverseYDirection(IndexParams, 0, 0, 7, SideMeshers, BorderIndexParams);

	// Test
	return true;
}

// TODO: remove
/*
FString Message;

for (int i = 0; i < RLEVoxelGrid->Num(); ++i)
{
	Message += TEXT("{");
	Message += FString::FromInt((*RLEVoxelGrid)[i].RunLenght);
	Message += TEXT(", FVoxel{");
	Message += FString::FromInt((*RLEVoxelGrid)[i].Voxel.VoxelId);
	Message += TEXT("}");
		
	Message += TEXT("}, ");
}

UE_LOG(LogTemp, Display, TEXT("%s"), *Message);
*/
