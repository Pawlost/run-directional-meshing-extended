#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "DespawnChunkSpawnerBase.generated.h"

/**
 * Spawner base allowing grid changes and chunk despawning
 */
UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API ADespawnChunkSpawnerBase : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 DespawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 ChunksAboveSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 ChunksBelowSpawner = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 BufferZone = 1;
	
	UFUNCTION(BlueprintCallable)
	void ChangeGridCenterToPosition(const FVector& NewPosition);

	UFUNCTION()
	virtual void DespawnChunks();
	

protected:
	virtual void BeginPlay() override;

	virtual void SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* OutAsyncExecution = nullptr) override;
	
	// Pool of despawned chunks
	TQueue<TSharedPtr<FChunk>> DespawnedChunks;
	
	FCriticalSection CriticalSection;
	
	TSharedFuture<void> DespawnHandle;
};
