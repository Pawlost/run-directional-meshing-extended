#include "DummyTestClasses.h"
#include "Misc/AutomationTest.h"
#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestValidVoxelSequenceAfterMeshing,
	"RDM.RDMMeshersTests.RLERunDirectionalVoxelMesher.CheckValidVoxelSequenceAfterMeshing",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
);

bool FRDMMeshersTests_RLERunDirectionalVoxelMesher_TestValidVoxelSequenceAfterMeshing::RunTest(const FString& Parameters)
{
	// Prepare
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<UDummyRLERunDirectionalVoxelMesher>();
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
	UDummyRLERunDirectionalVoxelMesher* DummyRLERunDirectionalVoxelMesher = NewObject<UDummyRLERunDirectionalVoxelMesher>();
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
