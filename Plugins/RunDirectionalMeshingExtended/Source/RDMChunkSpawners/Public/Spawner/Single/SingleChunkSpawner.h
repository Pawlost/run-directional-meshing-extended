#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "SingleChunkSpawner.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), Blueprintable)
class RDMCHUNKSPAWNERS_API ASingleChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void StartMeshing(TArray<FRLEVoxelEdit>& VoxelChange) override;
};
