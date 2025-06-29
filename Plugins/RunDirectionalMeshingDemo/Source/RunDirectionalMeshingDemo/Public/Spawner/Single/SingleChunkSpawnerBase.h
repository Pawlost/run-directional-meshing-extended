#pragma once

#include "CoreMinimal.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "SingleChunkSpawnerBase.generated.h"

struct FVoxelPosition;
struct FVoxelChange;

UCLASS(ClassGroup=(ChunkSpawners), Abstract)
class RUNDIRECTIONALMESHINGDEMO_API ASingleChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool AlignGridPositionWithSpawner = true;

	virtual void ChangeVoxelInChunk(const FVoxelPosition& VoxelPosition,
									const FName& VoxelId) override;
	
	virtual FName GetVoxelFromChunk(const FVoxelPosition& VoxelPosition) override;
	
	virtual TSharedFuture<void> SpawnChunksAsync() override;
	
protected:
	virtual void BeginPlay() override;
	virtual void StartMeshing(FVoxelChange* VoxelChange = nullptr) PURE_VIRTUAL(ASingleChunkSpawnerBase::StartMeshing)
	
	TSharedPtr<FChunk> SingleChunk;
};
