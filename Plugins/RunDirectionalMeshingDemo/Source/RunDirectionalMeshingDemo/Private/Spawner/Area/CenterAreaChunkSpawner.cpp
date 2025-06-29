#include "Spawner/Area/CenterAreaChunkSpawner.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"

void ACenterAreaChunkSpawner::GenerateArea()
{
	FMesherVariables MesherVars;
	MesherVars.ChunkParams.ShowBorders = BufferZone == 0;
	
	auto InitialCenter = CenterGridPosition;
	// Visited position will create with every change of grid center
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * CHUNK_FACE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnPositionsArray.Enqueue(InitialCenter);
	SpawnChunk(InitialCenter);
	TArray<TSharedFuture<void>> Tasks;
	Tasks.Reserve(LowerThreadLimit);

	TTuple<FFaceToDirection, int32> Directions[CHUNK_FACE_COUNT] = {
		TTuple<FFaceToDirection, int32>(FFaceToDirection::FrontDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::RightDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::LeftDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BackDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::TopDirection, ChunksAboveSpawner),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BottomDirection, ChunksBelowSpawner)
	};

	FIntVector CenterPosition;

	if (bSpawnCenterChunk)
	{
		VisitedSpawnPositions.Add(CenterGridPosition);
	}
	
	/*
	 * Use BFS to explore chunk locations around spawner in 3D and mesh them.
	 */
	while (IsValid(this) && SpawnPositionsArray.Dequeue(CenterPosition) && InitialCenter == CenterGridPosition)
	{
		if (Tasks.Num() >= LowerThreadLimit)
		{
			WaitForAllTasks(Tasks);
		}

		for (uint8 s = 0; s < CHUNK_FACE_COUNT; s++)
		{
			auto Direction = Directions[s];

			auto Position = CenterPosition + Direction.Key.Direction;

			auto ChunkPtr = ChunkGrid.Find(Position);

			if (FVector::Distance(FVector(InitialCenter), FVector(Position)) < Direction.Value + BufferZone)
			{
				if (ChunkPtr == nullptr)
				{
					TSharedFuture<void> Task;
					SpawnChunk(Position, &Task);

					if (Task.IsValid() && !Task.IsReady())
					{
						Tasks.Add(Task);
					}
				}

				if (VisitedSpawnPositions.Find(Position) == nullptr)
				{
					VisitedSpawnPositions.Add(Position);
					SpawnPositionsArray.Enqueue(Position);
				}
			}
		}

		// Mesh if certain distance
		if (FVector::Distance(FVector(InitialCenter), FVector(CenterPosition)) < MeshZone)
		{
			WaitForAllTasks(Tasks);
			GenerateChunkMesh(MesherVars, CenterPosition);
		}
	}
}
