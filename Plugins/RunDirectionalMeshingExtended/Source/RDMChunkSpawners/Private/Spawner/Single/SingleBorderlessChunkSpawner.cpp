#include "Spawner/Single/SingleBorderlessChunkSpawner.h"

void ASingleBorderlessChunkSpawner::StartMeshing(TArray<FRLEVoxelEdit>& VoxelChanges)
{
	FMesherVariables MesherVars;
	MesherVars.SpawnerPtr = this;
	MesherVars.OriginalChunk = SingleChunk;
	MesherVars.WorldTransform = UseWorldCenter;

	SpawnSideChunk(FFaceToDirection::TopDirection);
	SpawnSideChunk(FFaceToDirection::BottomDirection);
	SpawnSideChunk(FFaceToDirection::BackDirection);
	SpawnSideChunk(FFaceToDirection::FrontDirection);
	SpawnSideChunk(FFaceToDirection::RightDirection);
	SpawnSideChunk(FFaceToDirection::LeftDirection);
	
	MesherVars.OriginalChunk->ChunkMeshActor->SetVoxelGenerator(VoxelGenerator);
	MesherVars.OriginalChunk->ChunkMeshActor->GenerateMesh(MesherVars, VoxelChanges);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(const FFaceToDirection& FaceDirection )
{
	const auto Index = static_cast<uint8>(FaceDirection.FaceSide);
	auto Chunk = MakeShared<FChunk>().ToSharedPtr();
	SideChunk[Index] = Chunk;
	const auto GridPosition = SingleChunkGridPosition + FaceDirection.Direction;
	AddChunkToGrid(Chunk, GridPosition);
}