#include "Spawner/Single/SingleChunkSpawnerBase.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "Voxel/VoxelPosition.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "Voxel/Grid/VoxelModel.h"

void ASingleChunkSpawnerBase::BeginPlay()
{
	Super::BeginPlay();

	if (AlignGridPositionWithSpawner)
	{
		SingleChunkGridPosition = WorldPositionToChunkGridPosition(GetActorLocation());;
	}

	// Initialize single chunk
	SingleChunk = MakeShared<FChunk>();

	bIsInitialized = true;
	SpawnChunksAsync();
}

void ASingleChunkSpawnerBase::ChangeVoxelsInChunk(FCrossChunkEdit& ChunkEdits)
{
	for (auto ChunkEdit : ChunkEdits.VoxelEdits)
	{
		if (ChunkEdit.Key == SingleChunkGridPosition)
		{
			// Modify voxel at hit position
			StartMeshing(ChunkEdit.Value);
		}
	}
}

FName ASingleChunkSpawnerBase::GetVoxelFromChunk(const FVoxelPosition& VoxelPosition)
{
	const auto VoxelIndex = VoxelGenerator->CalculateVoxelIndex(VoxelPosition.VoxelPosition);
	const auto Voxel = SingleChunk->VoxelModel->GetVoxelAtIndex(VoxelIndex);
	const auto VoxelType = VoxelGenerator->GetVoxelTableRow(Voxel);
	return VoxelType.Key;
}

TSharedFuture<void> ASingleChunkSpawnerBase::SpawnChunksAsync()
{
	return Async(EAsyncExecution::Thread, [this]()
	{
		if (!SingleChunk->bIsActive){
			AddChunkToGrid(SingleChunk, SingleChunkGridPosition);
			SingleChunk->bIsActive = true;
		}
		
		TArray<FVoxelEdit> VoxelChanges;
		StartMeshing(VoxelChanges);
	}).Share();
}