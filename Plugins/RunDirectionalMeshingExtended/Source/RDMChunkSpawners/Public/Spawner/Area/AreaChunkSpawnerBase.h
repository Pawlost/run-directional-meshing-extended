#pragma once
#include "CoreMinimal.h"
#include "ChunkSpawnerBase.h"
#include "FaceToDirection.h"
#include "AreaChunkSpawnerBase.generated.h"

UCLASS(Abstract)
class RDMCHUNKSPAWNERS_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool bSpawnCenterChunk = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool bEnableInitialChunkSpawn = true;

	virtual void ApplyVoxelChanges(TMap<FIntVector, TArray<FRLEVoxelEdit>>& ChunkEdits) override;
	
	virtual TSharedFuture<void> SpawnChunksAsync() override;
	
protected:
	TMap<FIntVector, TSharedPtr<FChunk>> ChunkGrid;
	bool ShowChunkBorders = false;
	virtual void BeginPlay() override;

	TQueue<TWeakObjectPtr<AChunkActor>, EQueueMode::Mpsc> UnusedActorsPool;

	virtual void GenerateArea() PURE_VIRTUAL(AAreaChunkSpawnerBase::GenerateArea)
	void GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition, TArray<FRLEVoxelEdit>& VoxelEdits);
	virtual void SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* OutAsyncExecution = nullptr);

private:
	void AddChunksFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection);
	void DequeueChunkActor(TWeakObjectPtr<AChunkActor> ChunkActor);
	void EnqueueChunkActor(TWeakObjectPtr<AChunkActor> ChunkActor);
	
	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
