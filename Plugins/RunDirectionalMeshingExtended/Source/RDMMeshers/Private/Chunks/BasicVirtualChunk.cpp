#include "VoxelMesher/VirtualChunk/BasicVirtualChunk.h"
#include "VoxelMesher/BasicVoxelMesher.h"

static TArray<TSharedPtr<FBasicVoxelMesher>> UnusedMeshersPool;

FVoxel UBasicVirtualChunk::GetBorderVoxel(FBorderEventMesher& IndexParams,
                                          const FIntVector& BorderVoxelPosition)
{
	const auto VoxelIndex = VoxelData->CalculateVoxelIndex(BorderVoxelPosition);
	return (*BasicVoxelGridPtr)[VoxelIndex];
}

void UBasicVirtualChunk::CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid)
{
	this->BasicVoxelGridPtr = MakeShared<TArray<FVoxel>>(NewVoxelGrid);
}

void UBasicVirtualChunk::GenerateMesh(FVoxelMesh& MeshContainer, FChunkBorderContext& BorderParameters,
                                      TArray<FRLEVoxelEdit>& VoxelChanges)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RunDirectionalMeshing generation")
#endif
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Edit - RunDirectionalMeshing generation")
#endif

		FScopeLock Lock(&CriticalSection);
		while (!VoxelChanges.IsEmpty())
		{
			const auto& [EditEventIndex, EditVoxel] = VoxelChanges.Pop(EAllowShrinking::No);
			for (uint32 i = 0; i < EditVoxel.RunLenght; i++)
			{
				(*BasicVoxelGridPtr)[EditEventIndex + i] = EditVoxel.Voxel;
			}
		}
	}
	
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	TSharedPtr<FBasicVoxelMesher> EventPlanner;
	{
		// This scope may start in a parallel task
		FScopeLock Lock(&MesherCriticalSection);
		if (!UnusedMeshersPool.IsEmpty())
		{
			EventPlanner = UnusedMeshersPool.Pop();
		}
		else
		{
			EventPlanner = MakeShared<FBasicVoxelMesher>(VoxelData);
		}
	}

	EventPlanner->VoxelGrid = BasicVoxelGridPtr;

	EventPlanner->GenerateVirtualFaces(BorderParameters);
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