#include "Spawner/Single/SingleChunkSpawnerBase.h"
#include "VoxelMesher/MeshingUtils/VoxelEdit.h"

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

void ASingleChunkSpawnerBase::ApplyVoxelChanges(TMap<FIntVector, FChunkEdit>& ChunkEdits)
{
	for (auto ChunkEdit : ChunkEdits)
	{
		if (ChunkEdit.Key == SingleChunkGridPosition)
		{
			// Modify voxel at hit position
			StartMeshing(ChunkEdit.Value.VoxelEdits);
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

	TArray<FVoxelEdit> VoxelChanges;
	StartMeshing(VoxelChanges);
}
