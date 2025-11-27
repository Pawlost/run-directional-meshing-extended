#include "Spawner/Single/SingleChunkSpawnerBase.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	// Initialize single chunk
	SingleChunk = MakeShared<FChunk>();
	
	Super::BeginPlay();

	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = GetChunkGridPositionFromGlobalPosition(GetActorLocation());
	}
	
	bIsInitialized = true;
	SpawnChunks();
}

void ASingleChunkSpawnerBase::ApplyVoxelChanges(TMap<FIntVector, TArray<FRLEVoxelEdit>>& ChunkEdits)
{
	for (auto ChunkEdit : ChunkEdits)
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
		SpawnChunks();
	}).Share();
}

void ASingleChunkSpawnerBase::SpawnChunks()
{
	if (!SingleChunk->bIsActive)
	{
		AddChunkToGrid(SingleChunk, SingleChunkGridPosition);
		SingleChunk->bIsActive = true;
	}

	TArray<FRLEVoxelEdit> VoxelEdits;
	StartMeshing(VoxelEdits);
}
