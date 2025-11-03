#include "Spawner/Single/SingleChunkSpawnerBase.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();

	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = GetChunkGridPositionFromGlobalPosition(GetActorLocation());
	}

	// Initialize single chunk
	SingleChunk = MakeShared<FChunk>();

	bIsInitialized = true;
	SpawnChunksAsync();
}

void ASingleChunkSpawnerBase::ChangeVoxelsInChunk(FCrossChunkEdit& ChunkEdits)
{
	for (auto ChunkEdit : ChunkEdits.VoxelEdits)
	{
		if (ChunkEdit.Key == SingleChunkGridPosition)
		{
			// Modify voxel at hit position
			StartMeshing(ChunkEdit.Value);
		}
	}
}

TSharedFuture<void> ASingleChunkSpawnerBase::SpawnChunksAsync()
{
	return Async(EAsyncExecution::Thread, [this]()
	{
		if (!SingleChunk->bIsActive)
		{
			AddChunkToGrid(SingleChunk, SingleChunkGridPosition);
			SingleChunk->bIsActive = true;
		}

		TArray<FVoxelEdit> VoxelChanges;
		StartMeshing(VoxelChanges);
	}).Share();
}
