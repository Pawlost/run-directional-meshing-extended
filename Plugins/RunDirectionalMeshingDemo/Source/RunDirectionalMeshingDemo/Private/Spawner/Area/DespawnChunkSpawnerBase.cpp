#include "Spawner/Area/DespawnChunkSpawnerBase.h"

#include "Chunk/Chunk.h"

void ADespawnChunkSpawnerBase::ChangeGridCenterToPosition(const FVector& NewPosition)
{
	const auto NewGridPosition = WorldPositionToChunkGridPosition(NewPosition);

	// Restart spawning if center moved
	if (CenterGridPosition != NewGridPosition)
	{
		CenterGridPosition = NewGridPosition;
		SpawnChunksAsync();
		DespawnChunks();
	}
}

void ADespawnChunkSpawnerBase::BeginPlay()
{
	ShowChunkBorders = BufferZone == 0;
	Super::BeginPlay();
}

void ADespawnChunkSpawnerBase::DespawnChunks()
{
	if (DespawnHandle.IsValid() && !DespawnHandle.IsReady())
	{
		// Limit despawn to only one thread
		return;
	}

	DespawnHandle = Async(EAsyncExecution::Thread, [this]()
	{
		TArray<FIntVector> ChunkKeys;
		ChunkGrid.GetKeys(ChunkKeys);

		for (auto ChunkKey : ChunkKeys)
		{
			if (FVector::Distance(FVector(CenterGridPosition), FVector(ChunkKey)) > SpawnZone + DespawnZone)
			{
				if (!IsValid(this) || !ChunkGrid.Contains(ChunkKey))
				{
					return;
				}

				auto Chunk = *ChunkGrid.Find(ChunkKey);

				if (Chunk->ChunkMeshActor != nullptr)
				{
					Chunk->ChunkMeshActor->ClearMesh();
					UnusedActorsPool.Enqueue(Chunk->ChunkMeshActor);
					Chunk->ChunkMeshActor = nullptr;
				}

				Chunk->bIsActive = false;
				Chunk->ChunkVoxelIdTable.Reset();

				CriticalSection.Lock();
				ChunkGrid.Remove(ChunkKey);
				CriticalSection.Unlock();

				/*
				 * When chunk is despawned move it to the pool
				 * There is so far not a limit on pool size because this is a demo application
				 */
				DespawnedChunks.Enqueue(Chunk);
			}
		}
	});
}

void ADespawnChunkSpawnerBase::SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* AsyncExecution)
{
	if (!IsValid(this) || ChunkGrid.Contains(ChunkGridPosition))
	{
		// Don't spawn chunk if there is one located at the grid position
		return;
	}

	// Try to get chunk from chunk pool
	TSharedPtr<FChunk> Chunk;
	if (!DespawnedChunks.Dequeue(Chunk))
	{
		Chunk = MakeShared<FChunk>();
	}
	
	AddChunkToGrid(Chunk, ChunkGridPosition, AsyncExecution);

	CriticalSection.Lock();
	ChunkGrid.Add(ChunkGridPosition, Chunk);
	CriticalSection.Unlock();
}
