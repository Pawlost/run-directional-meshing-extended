#pragma once

#include "CoreMinimal.h"
#include "ChunkSpawnerBase.h"
#include "SingleChunkSpawnerBase.generated.h"

struct FVoxelPosition;

UCLASS(ClassGroup=(ChunkSpawners), Abstract)
class RDMCHUNKSPAWNERS_API ASingleChunkSpawnerBase : public AChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	FIntVector SingleChunkGridPosition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Chunk")
	bool AlignGridPositionWithSpawner = true;

	virtual void ApplyVoxelChanges(TMap<FIntVector, TArray<FRLEVoxelEdit>>&  ChunkEdits) override;
	
	virtual TSharedFuture<void> SpawnChunksAsync() override;
	
	virtual void SpawnChunks();
	
protected:
	virtual void BeginPlay() override;
	virtual void StartMeshing(TArray<FRLEVoxelEdit>& VoxelChange) PURE_VIRTUAL(ASingleChunkSpawnerBase::StartMeshing)
	
	TSharedPtr<FChunk> SingleChunk;
};
