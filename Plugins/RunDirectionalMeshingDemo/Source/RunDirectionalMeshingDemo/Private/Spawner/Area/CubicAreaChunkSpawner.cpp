#include "Spawner/Area/CubicAreaChunkSpawner.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"

void ACubicAreaChunkSpawner::GenerateArea()
{
	const auto MinPosition = CenterGridPosition - FIntVector(SpawnZone, SpawnZone, ChunksBelowSpawner);
	const auto MaxPosition = CenterGridPosition + FIntVector(SpawnZone, SpawnZone, ChunksAboveSpawner);

	// Fill chunks with voxels from left bottom to right top
	for (int32 x = MinPosition.X; x < MaxPosition.X; x++)
	{
		for (int32 y = MinPosition.Y; y < MaxPosition.Y; y++)
		{
			for (int32 z = MinPosition.Z; z < MaxPosition.Z; z++)
			{
				auto CurrentGridPosition = FIntVector(x, y, z);
				SpawnChunk(CurrentGridPosition);
			}
		}
	}
	
	// Create meshes from left bottom to right top
	FMesherVariables MeshVars;
	for (int32 x = MinPosition.X; x < MaxPosition.X; x++)
	{
		for (int32 y = MinPosition.Y; y < MaxPosition.Y; y++)
		{
			for (int32 z = MinPosition.Z; z < MaxPosition.Z; z++)
			{
				auto CurrentGridPosition = FIntVector(x, y, z);
				GenerateChunkMesh(MeshVars, CurrentGridPosition);
			}
		}
	}
}
