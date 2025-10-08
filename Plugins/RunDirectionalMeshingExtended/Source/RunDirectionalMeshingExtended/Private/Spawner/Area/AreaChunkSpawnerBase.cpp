﻿#include "Spawner/Area/AreaChunkSpawnerBase.h"
#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "Voxel/VoxelPosition.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "Voxel/Grid/VoxelModel.h"

void AAreaChunkSpawnerBase::ChangeVoxelInChunk(const FVoxelPosition& VoxelPosition,
                                               const FName& VoxelName)
{
	int ChunkDimensions = VoxelGenerator->GetVoxelCountPerVoxelLine();
	if (VoxelPosition.VoxelPosition.X < 0 || VoxelPosition.VoxelPosition.Y < 0 || VoxelPosition.VoxelPosition.Z < 0 ||
		VoxelPosition.VoxelPosition.X >= ChunkDimensions || VoxelPosition.VoxelPosition.Y >= ChunkDimensions || VoxelPosition.VoxelPosition.Z >= ChunkDimensions)
	{
		return;
	}
	
	if (EditHandle.IsValid() && !EditHandle.IsReady())
	{
		return;
	}

	if (ChunkGrid.Contains(VoxelPosition.ChunkGridPosition))
	{
		const auto FoundChunk = ChunkGrid.Find(VoxelPosition.ChunkGridPosition);

		if (FoundChunk == nullptr || !FoundChunk->IsValid())
		{
			return;
		}

		auto Chunk = *FoundChunk;
		
		FMesherVariables MesherVars;
		Chunk->bIsActive = false;
		FVoxelChange Modification(VoxelName, VoxelPosition.VoxelPosition);
		TArray<FVoxelChange> VoxelChanges;
		VoxelChanges.Add(Modification);
		GenerateChunkMesh(MesherVars, Chunk->GridPosition, VoxelChanges);
		
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

FName AAreaChunkSpawnerBase::GetVoxelFromChunk(const FVoxelPosition& VoxelPosition)
{
	const auto ChunkPtr = ChunkGrid.Find(VoxelPosition.ChunkGridPosition);

	if (ChunkPtr != nullptr){
		const auto Chunk = *ChunkPtr;
		const auto VoxelIndex = VoxelGenerator->CalculateVoxelIndex(VoxelPosition.VoxelPosition);
		const auto Voxel = Chunk->VoxelModel->GetVoxelAtIndex(VoxelIndex);
		if (!Voxel.IsEmptyVoxel()){
			const auto VoxelType = VoxelGenerator->GetVoxelTypeById(Voxel);
			return VoxelType.Key;
		}
	}
	
	return FName();
}

void AAreaChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();
	checkf(VoxelGenerator, TEXT("Voxel generator must set"));

	if (UseWorldCenter)
	{
		CenterGridPosition = WorldPositionToChunkGridPosition(GetTransform().GetLocation());
	}

	if (bEnableInitialChunkSpawn){
		if (bSpawnCenterChunk)
		{
			//Spawn center chunk
			SpawnChunk(CenterGridPosition);
			FMesherVariables MesherVars;
			auto VoxelChanges = TArray<FVoxelChange>();
			GenerateChunkMesh(MesherVars, CenterGridPosition, VoxelChanges);
		}

		SpawnChunksAsync();
	}
}

//Running on main thread may cause deadlock
void AAreaChunkSpawnerBase::GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition, TArray<FVoxelChange>& VoxelChanges)
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

	MesherVars.ChunkParams.SpawnerPtr = this;
	MesherVars.ChunkParams.OriginalChunk = Chunk;
	MesherVars.ChunkParams.ShowBorders = ShowChunkBorders;
	MesherVars.ChunkParams.WorldTransform = UseWorldCenter;

	AddChunkFromGrid(MesherVars, FFaceToDirection::TopDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BottomDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::RightDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::LeftDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::FrontDirection);
	AddChunkFromGrid(MesherVars, FFaceToDirection::BackDirection);

	DequeueChunkActor(Chunk->ChunkMeshActor);
	DequeueChunkActor(Chunk->BorderChunkMeshActor);
	
	auto Spawner = MakeShared<FChunkParams>(MesherVars.ChunkParams);
	
	if (IsInGameThread())
	{
		//Creating AsyncTask from main thread will cause deadlock
		SpawnAndMoveChunkActor(Spawner, Chunk->ChunkMeshActor);
		SpawnAndMoveChunkActor(Spawner, Chunk->BorderChunkMeshActor);
	}
	else
	{
		// Synchronize Mesh generation with game thread.
		Async(EAsyncExecution::TaskGraphMainThread, [this, Spawner]()
		{
			SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->ChunkMeshActor);
			SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->BorderChunkMeshActor);
		}).Wait();
	}

	//Mesh could be spawned on a Async Thread similarly to voxel models but it is not done so to showcase real time speed of mesh generation (requirement for bachelor thesis)
	VoxelGenerator->GenerateMesh(MesherVars, VoxelChanges);

	if (!Chunk->bHasMesh)
	{
		UnusedActorsPool.Enqueue(Chunk->ChunkMeshActor);
		Chunk->ChunkMeshActor = nullptr;
	}

	for (auto SideChunk : MesherVars.ChunkParams.SideChunks)
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

void AAreaChunkSpawnerBase::AddChunkFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection)
{
	const auto Chunk = ChunkGrid.Find(MesherVars.ChunkParams.OriginalChunk->GridPosition + FaceDirection.Direction);
	if (Chunk == nullptr)
	{
		AddSideChunk(MesherVars, FaceDirection.FaceSide, nullptr);
	}
	else
	{
		AddSideChunk(MesherVars, FaceDirection.FaceSide, *Chunk);
	}
}

void AAreaChunkSpawnerBase::DequeueChunkActor(TWeakObjectPtr<AChunkActor> ChunkActor)
{
	if (ChunkActor == nullptr)
	{
		UnusedActorsPool.Dequeue(ChunkActor);
	}
}

TSharedFuture<void> AAreaChunkSpawnerBase::SpawnChunksAsync()
{
	return Async(EAsyncExecution::Thread, [this]()
	{
		GenerateArea();
	}).Share();
}
