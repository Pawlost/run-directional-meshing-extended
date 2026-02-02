#include "VoxelMesher/VirtualChunk/RLEVirtualChunk.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelMesher/VoxelEventMesher.h"

TArray<TSharedPtr<FVoxelEventMesher>> URLEVirtualChunk::UnusedMeshersPool;

void URLEVirtualChunk::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	if (VoxelGrid.Num() != 0 && RLEVoxelGrid == nullptr)
	{
		const auto NewRLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

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
void URLEVirtualChunk::GenerateMesh(FVoxelMesh& MeshContainer, FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
{
	// This scope may start in a parallel task
	
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif
	
	const uint32 MaxVoxelsInChunk = VoxelData->GetMaxVoxelsInChunk();
	const uint32 VoxelLine = VoxelData->GetVoxelLine();
	const uint32 VoxelPlane = VoxelData->GetVoxelPlane();
	
	TSharedPtr<FVoxelEventMesher> EventPlanner;
	{
		FScopeLock Lock(&MesherCriticalSection);
		if (!UnusedMeshersPool.IsEmpty())
		{
			EventPlanner = UnusedMeshersPool.Pop();
		}else
		{
			EventPlanner = MakeShared<FVoxelEventMesher>(VoxelData);
		}
	}
	
	EventPlanner->UpdateInternalState(VoxelLine, VoxelPlane, MaxVoxelsInChunk);
	// Keeps all variables local inside the task scope
	TSharedPtr<TArray<FRLEVoxel>> VoxelGridCopy;
	{
		FScopeLock Lock(&GridCriticalSection);
		VoxelGridCopy = RLEVoxelGrid;
	}
	
	EventPlanner->InitializeIntervals(VoxelGridCopy, VoxelEdits);
	EventPlanner->GenerateVirtualFaces(BorderParameters, VoxelEdits);
	
	if (EventPlanner->IsEditEnabled())
	{
		FScopeLock Lock(&GridCriticalSection);
		RLEVoxelGrid = EventPlanner->GetMainVoxelGridPtr();
	}
	
	EventPlanner->ConvertVirtualFacesToMesh(MeshContainer, VoxelData->VoxelSize);
	
	{
		FScopeLock Lock(&MesherCriticalSection);
		constexpr int MAX_NUMBER_OF_MESHERS = 20;
		if (UnusedMeshersPool.Num() < MAX_NUMBER_OF_MESHERS)
		{
			UnusedMeshersPool.Push(EventPlanner);
		}
	}
}

FVoxel URLEVirtualChunk::GetBorderVoxel(FBorderEventMesher& BorderMeshingEventPlanner, const FIntVector& BorderVoxelPosition)
{
	const uint32 MaxChunkVoxelSequence = VoxelData->GetMaxVoxelsInChunk();

	auto& VoxelGridPtr = BorderMeshingEventPlanner.BorderMeshingEvent.VoxelGridPtr;
	if (VoxelGridPtr == nullptr)
	{
		VoxelGridPtr = RLEVoxelGrid;
	}

	const uint32 NextIndex = VoxelData->CalculateVoxelIndex(BorderVoxelPosition);
	auto& BorderMeshingEvent = BorderMeshingEventPlanner.BorderMeshingEvent;

	while (BorderMeshingEventPlanner.GetVoxelIndex() < NextIndex)
	{
		BorderMeshingEventPlanner.SetVoxelIndex(BorderMeshingEventPlanner.NextVoxelIndex);
		BorderMeshingEventPlanner.NextVoxelIndex = MaxChunkVoxelSequence;
		BorderMeshingEventPlanner.AdvanceMeshingEvent(BorderMeshingEvent);
		BorderMeshingEventPlanner.TryUpdateNextMeshingEvent(NextIndex);
	}

	return BorderMeshingEvent.GetCurrentVoxel();
}