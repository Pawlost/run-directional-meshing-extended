#pragma once
#include "CoreMinimal.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "AreaChunkSpawnerBase.generated.h"

struct FVoxelPosition;
struct FVoxelChange;
struct FFaceToDirection;
class AChunkRMCActor;

UCLASS(Abstract)
class RUNDIRECTIONALMESHINGDEMO_API AAreaChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Chunk")
	int32 SpawnZone = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool bSpawnCenterChunk = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool bEnableInitialChunkSpawn = true;

	virtual void ChangeVoxelInChunk(const FVoxelPosition& VoxelPosition,
	                                const FName& VoxelName) override;

	virtual FName GetVoxelFromChunk(const FVoxelPosition& VoxelPosition) override;
	
	virtual TSharedFuture<void> SpawnChunksAsync() override;
	
protected:
	TMap<FIntVector, TSharedPtr<FChunk>> ChunkGrid;
	bool ShowChunkBorders = false;
	virtual void BeginPlay() override;

	TQueue<TWeakObjectPtr<AChunkRMCActor>, EQueueMode::Mpsc> UnusedActorsPool;

	virtual void GenerateArea() PURE_VIRTUAL(AAreaChunkSpawnerBase::GenerateArea)
	void GenerateChunkMesh(FMesherVariables& MesherVars, const FIntVector& ChunkGridPosition, FVoxelChange* VoxelChange = nullptr);
	virtual void SpawnChunk(const FIntVector& ChunkGridPosition, TSharedFuture<void>* OutAsyncExecution = nullptr);



private:
	void AddChunkFromGrid(FMesherVariables& MesherVars, const FFaceToDirection& FaceDirection);

	TSharedFuture<void> EditHandle;
	FCriticalSection Mutex;
};
