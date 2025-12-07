#include "Spawner/Area/AreaChunkSpawnerBase.h"

void AAreaChunkSpawnerBase::ApplyVoxelChanges(TMap<FIntVector, TArray<FRLEVoxelEdit>>& ChunkEdits)
{
	if (EditHandle.IsValid() && !EditHandle.IsReady())
	{
		return;
	}

	for (auto ChunkEdit : ChunkEdits)
	{
		if (ChunkGrid.Contains(ChunkEdit.Key))
		{
			const auto FoundChunk = ChunkGrid.Find(ChunkEdit.Key);

			if (FoundChunk == nullptr || !FoundChunk->IsValid())
			{
				return;
			}

			const auto Chunk = *FoundChunk;

			FMesherVariables MesherVars;
			Chunk->bIsActive = false;
			GenerateChunkMesh(MesherVars, Chunk->GridPosition, ChunkEdit.Value);

			// TODO: rewrite edit

			/*EditHandle = Async(EAsyncExecution::ThreadPool, [this, MesherVars]()
			{
				FMesherVariables SideMesherVars;
				for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
				{
					auto SideChunk = MesherVars.ChunkParams.SideChunks[s];
					if (SideChunk.IsValid())
					{
						SideChunk->bIsActive = false;
						TArray<FVoxelChange> VoxelChanges;
						GenerateChunkMesh(SideMesherVars, SideChunk->GridPosition, VoxelChanges);
					}
				}
			}).Share();*/
		}
	}
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	checkf(VoxelGenerator, TEXT("Voxel generator must set"));

	if (UseWorldCenter)
	{
		CenterGridPosition = GetChunkGridPositionFromGlobalPosition(GetTransform().GetLocation());
	}

	if (bEnableInitialChunkSpawn)
	{
		if (bSpawnCenterChunk)
		{
			//Spawn center chunk
			SpawnChunk(CenterGridPosition);
			FMesherVariables MesherVars;
			auto VoxelChanges = TArray<FRLEVoxelEdit>();
			GenerateChunkMesh(MesherVars, CenterGridPosition, VoxelChanges);
		}

		SpawnChunksAsync();
	}
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition,
                                              TArray<FRLEVoxelEdit>& VoxelEdits)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Area Mesh generation prepartion")
#endif

	if (!IsValid(this) || !ChunkGrid.Contains(ChunkGridPosition))
	{
		return;
	}

	const TSharedPtr<FChunk>& Chunk = *ChunkGrid.Find(ChunkGridPosition);

	if (Chunk->bIsActive)
	{
		return;
	}

	MesherVars.SpawnerPtr = this;
	MesherVars.OriginalChunk = Chunk;
	MesherVars.ShowBorders = ShowChunkBorders;
	MesherVars.WorldTransform = UseWorldCenter;

	AddChunksFromGrid(MesherVars, FFaceToDirection::TopDirection);
	AddChunksFromGrid(MesherVars, FFaceToDirection::BottomDirection);
	AddChunksFromGrid(MesherVars, FFaceToDirection::RightDirection);
	AddChunksFromGrid(MesherVars, FFaceToDirection::LeftDirection);
	AddChunksFromGrid(MesherVars, FFaceToDirection::FrontDirection);
	AddChunksFromGrid(MesherVars, FFaceToDirection::BackDirection);

	DequeueChunkActor(Chunk->ChunkMeshActor);
	for (int i = 0; i < CHUNK_FACE_COUNT; ++i)
	{
		DequeueChunkActor(Chunk->BorderChunkMeshActor[i]);
	}

	auto Spawner = MakeShared<FMesherVariables>(MesherVars);

	if (IsInGameThread())
	{
		SpawnChunkActors(Spawner);
	}
	else
	{
		// Synchronize Mesh generation with game thread.
		Async(EAsyncExecution::TaskGraphMainThread, [this, Spawner]()
		{
			SpawnChunkActors(Spawner);
		}).Wait();
	}

	//Mesh could be spawned on a Async Thread similarly to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(MesherVars, VoxelEdits);

	EnqueueChunkActor(Chunk->ChunkMeshActor);
	for (int i = 0; i < CHUNK_FACE_COUNT; ++i)
	{
		EnqueueChunkActor(Chunk->BorderChunkMeshActor[i]);
	}

	for (auto SideChunk : MesherVars.SideChunks)
	{
		if (SideChunk == nullptr || !SideChunk.IsValid())
		{
			// If not all side chunks were available the chunk is not fully active and needs to be remeshed
			return;
		}
	}

	Chunk->bIsActive = true;
}

void AAreaChunkSpawnerBase::SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* AsyncExecution)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Chunk spawning generation");
#endif

	if (!IsValid(this) || ChunkGrid.Contains(ChunkGridPosition))
	{
		// Don't spawn chunk if there is one located at the grid position
		return;
	}

	TSharedPtr<FChunk> Chunk = MakeShared<FChunk>();
	AddChunkToGrid(Chunk, ChunkGridPosition, AsyncExecution);

	Mutex.Lock();
	ChunkGrid.Add(ChunkGridPosition, Chunk);
	Mutex.Unlock();
}

void AAreaChunkSpawnerBase::AddChunksFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection)
{
	const auto ChunkPtr = ChunkGrid.Find(MesherVars.OriginalChunk->GridPosition + FaceDirection.Direction);
	
	const auto DirectionIndex = static_cast<uint8>(FaceDirection.FaceSide);
	
	if (ChunkPtr != nullptr)
	{
		auto& Chunk = *ChunkPtr;
		MesherVars.SideChunks[DirectionIndex] = Chunk.IsValid() ? Chunk : nullptr;
	}
}

void AAreaChunkSpawnerBase::DequeueChunkActor(TWeakObjectPtr<AChunkActor> ChunkActor)
{
	if (ChunkActor == nullptr)
	{
		UnusedActorsPool.Dequeue(ChunkActor);
	}
}

void AAreaChunkSpawnerBase::EnqueueChunkActor(TWeakObjectPtr<AChunkActor> ChunkActor)
{
	if (ChunkActor.IsValid() && !ChunkActor->HasMesh())
	{
		UnusedActorsPool.Enqueue(ChunkActor);
		ChunkActor = nullptr;
	}
}

TSharedFuture<void> AAreaChunkSpawnerBase::SpawnChunksAsync()
{
	return Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	}).Share();
}
