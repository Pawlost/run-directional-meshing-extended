#include "Spawner/Single/SingleBorderlessChunkSpawner.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"

void ASingleBorderlessChunkSpawner::StartMeshing(FVoxelChange* VoxelChange)
{
	FMesherVariables ChunkParams;
	ChunkParams.ChunkParams.SpawnerPtr = this;
	ChunkParams.ChunkParams.OriginalChunk = SingleChunk;
	ChunkParams.ChunkParams.WorldTransform = UseWorldCenter;

	SpawnSideChunk(ChunkParams, FFaceToDirection::TopDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::BottomDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::BackDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::FrontDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::RightDirection);
	SpawnSideChunk(ChunkParams, FFaceToDirection::LeftDirection);

	VoxelGenerator->GenerateMesh(ChunkParams, VoxelChange);
}

void ASingleBorderlessChunkSpawner::SpawnSideChunk(FMesherVariables& MeshVars, const FFaceToDirection& FaceDirection )
{
	const auto Index = static_cast<uint8>(FaceDirection.FaceSide);
	auto Chunk = MakeShared<FChunk>().ToSharedPtr();
	SideChunk[Index] = Chunk;
	const auto GridPosition = SingleChunkGridPosition + FaceDirection.Direction;
	AddChunkToGrid(Chunk, GridPosition);
	AddSideChunk(MeshVars, FaceDirection.FaceSide, SideChunk[Index]);
}
