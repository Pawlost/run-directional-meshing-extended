#include "Spawner/Area/PreloadedVoxelCenterAreaChunkSpawner.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"

void APreloadedVoxelCenterAreaChunkSpawner::GenerateArea()
{
	auto InitialCenter = CenterGridPosition;
	TSet<FIntVector> VisitedSpawnPositions;
	VisitedSpawnPositions.Reserve(SpawnZone * SpawnZone * SpawnZone * CHUNK_FACE_COUNT);
	TQueue<FIntVector> SpawnPositionsArray;
	SpawnChunk(InitialCenter);
	TArray<TSharedFuture<void>> Tasks;
	Tasks.Reserve(6);

	SpawnPositionsArray.Enqueue(InitialCenter);

	TTuple<FFaceToDirection, int32> Directions[6] = {
		TTuple<FFaceToDirection, int32>(FFaceToDirection::FrontDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::RightDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::LeftDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BackDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::TopDirection, SpawnZone),
		TTuple<FFaceToDirection, int32>(FFaceToDirection::BottomDirection, SpawnZone)
	};

	FIntVector CenterPosition;
	VisitedSpawnPositions.Add(CenterGridPosition);

	/*
	 * Use BFS to explore chunk locations around spawner in 3D.
	 * 
	 */
	while (IsValid(this) && SpawnPositionsArray.Dequeue(CenterPosition) && InitialCenter == CenterGridPosition)
	{
		for (int32 s = 0; s < CHUNK_FACE_COUNT; s++)
		{
			auto Direction = Directions[s];

			auto Position = CenterPosition + Direction.Key.Direction;

			auto ChunkPtr = ChunkGrid.Find(Position);

			if (FVector::Distance(FVector(InitialCenter), FVector(Position)) < Direction.Value)
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

		if (Tasks.Num() >= 6)
		{
			WaitForAllTasks(Tasks);
		}
	}

	WaitForAllTasks(Tasks);
	
	if(!IsValid(this)){
		return;
	}

	//Foreach visited and spawned chunk create mesh
	FMesherVariables MeshVars;
	for (auto VisitedSpawnPosition : VisitedSpawnPositions)
	{
		GenerateChunkMesh(MeshVars, VisitedSpawnPosition);
	}
}

void APreloadedVoxelCenterAreaChunkSpawner::BeginPlay()
{
	ShowChunkBorders = true;
	Super::BeginPlay();
}
