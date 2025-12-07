#include "Spawner/Single/SingleChunkSpawner.h"

void ASingleChunkSpawner::BeginPlay()
{
	UseWorldCenter = true;
	Super::BeginPlay();
}

void ASingleChunkSpawner::StartMeshing(TArray<FRLEVoxelEdit>& VoxelChange)
{
	FMesherVariables MeshVar;
	MeshVar.SpawnerPtr = this;
	MeshVar.OriginalChunk = SingleChunk;
	
	auto Spawner = MakeShared<FMesherVariables>(MeshVar);

	if (IsInGameThread())
	{
		//Creating AsyncTask from main thread will cause deadlock
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
	
	MeshVar.ShowBorders = true;
	MeshVar.WorldTransform = UseWorldCenter;
	VoxelGenerator->GenerateMesh(MeshVar, VoxelChange);
}