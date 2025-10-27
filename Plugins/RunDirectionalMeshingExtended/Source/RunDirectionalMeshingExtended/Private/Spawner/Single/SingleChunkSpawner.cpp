#include "Spawner/Single/SingleChunkSpawner.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"

void ASingleChunkSpawner::BeginPlay()
{
	UseWorldCenter = true;
	Super::BeginPlay();
}

void ASingleChunkSpawner::StartMeshing(TArray<FVoxelEdit>& VoxelChange)
{
	FMesherVariables MeshVar;
	MeshVar.ChunkParams.SpawnerPtr = this;
	MeshVar.ChunkParams.OriginalChunk = SingleChunk;
	
	auto Spawner = MakeShared<FChunkParams>(MeshVar.ChunkParams);

	// TODO: rewrite
	if (IsInGameThread())
	{
		//Creating AsyncTask from main thread will cause deadlock
		SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->ChunkMeshActor);
		SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->BorderChunkMeshActor);
	}
	else
	{
		// Synchronize Mesh generation with game thread.
		Async(EAsyncExecution::TaskGraphMainThread, [this, Spawner]()
		{
			// TODO: rewrite
			SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->ChunkMeshActor);
			SpawnAndMoveChunkActor(Spawner, Spawner->OriginalChunk->BorderChunkMeshActor);
		}).Wait();
	}
	
	AddSideChunk(MeshVar, EFaceDirection::Top, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Bottom, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Front, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Back, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Right, nullptr);
	AddSideChunk(MeshVar, EFaceDirection::Left, nullptr);
	MeshVar.ChunkParams.ShowBorders = true;
	MeshVar.ChunkParams.WorldTransform = UseWorldCenter;
	VoxelGenerator->GenerateMesh(MeshVar, VoxelChange);
}