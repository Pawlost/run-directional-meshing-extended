#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"

void URLERunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	if (VoxelGrid.Num() != 0 && RLEVoxelGrid == nullptr)
	{
		auto NewRLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

		// Seed with first voxel run
		NewRLEVoxelGrid->Emplace(1, VoxelGrid[0]);

		for (int32 x = 1; x < VoxelGrid.Num(); x++)
		{
			const FVoxel& Voxel = VoxelGrid[x];
			if (NewRLEVoxelGrid->Last().Voxel == Voxel)
			{
				NewRLEVoxelGrid->Last().RunLenght++;
			}
			else
			{
				NewRLEVoxelGrid->Emplace(1, Voxel);
			}
		}

		RLEVoxelGrid = NewRLEVoxelGrid;
	}
}

// persistent preallocation must be maintained
void URLERunDirectionalVoxelMesher::GenerateMesh(
                                                 TStaticArray<
	                                                 TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>&
                                                 VirtualFaces,
                                                 TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>>& LocalVoxelTable,
                                                 TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
                                                 TArray<FRLEVoxelEdit>& VoxelChanges,
                                                 TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>
                                                 SideFaces,
                                                 TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>&
                                                 SideMeshers, bool ShowBorders)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	FaceGeneration(VoxelChanges, VirtualFaces, SideFaces, SideMeshers, ShowBorders);

	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - Directional Greedy Merge");
#endif
		
		constexpr int EstimatedRows = 3;
		
		//TODO: keep allocations persistent
		TArray<FVirtualVoxelFace> FirstArray;
		FirstArray.Reserve(ChunkDimension * EstimatedRows);

		TArray<FVirtualVoxelFace> SecondArray;
		SecondArray.Reserve(ChunkDimension * EstimatedRows);

		for (int f = 0; f < CHUNK_FACE_COUNT; f++)
		{
			for (uint32 y = 0; y < ChunkDimension; y++)
			{
				DirectionalGreedyMerge(FirstArray, SecondArray, LocalVoxelTable,
				                       FaceTemplates[f].StaticMeshingData, (*VirtualFaces[f])[y]);
			}

			DirectionalGreedyMerge(FirstArray, SecondArray, LocalVoxelTable,
			                       FaceTemplates[f].StaticMeshingData, *SideFaces[f]);
		}
	}
}


void URLERunDirectionalVoxelMesher::FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits,
                                                   TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
                                                   TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
                                                   TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers, bool ShowBorders)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif


	const uint32 MaxVoxelsInChunk = VoxelData->GetVoxelCountPerChunk();
	const uint32 VoxelLine = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlane = VoxelData->GetVoxelCountPerVoxelPlane();

	VirtualMeshEventPlanner EventPlanner(VoxelLine, VoxelPlane, MaxVoxelsInChunk, VirtualFaces, SideFaces, ShowBorders);
	

	EventPlanner.InitializeIntervals(RLEVoxelGrid, VoxelEdits);
	
	TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT> BorderIndexParams;

	EventPlanner.GenerateVirtualFaces();
	
	if (EventPlanner.IsEditEnabled())
	{
		RLEVoxelGrid =  EventPlanner.GetMainVoxelGridPtr();
	}
}

FVoxel URLERunDirectionalVoxelMesher::GetBorderVoxel(VirtualMeshEventPlanner& IndexParams, int X, int Y, int Z)
{
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();

	auto& VoxelGridPtr = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	if (VoxelGridPtr == nullptr)
	{
		VoxelGridPtr = RLEVoxelGrid;
	}

	uint32 NextIndex = VoxelData->CalculateVoxelIndex(X, Y, Z);

	while (IndexParams.CurrentMeshingEventIndex < NextIndex)
	{
		IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
		IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;
		AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval);
		IndexParams.TryUpdateNextMeshingEvent(NextIndex);
	}

	return IndexParams.MeshingEvents[LeadingInterval].GetCurrentVoxel().Voxel;
}
