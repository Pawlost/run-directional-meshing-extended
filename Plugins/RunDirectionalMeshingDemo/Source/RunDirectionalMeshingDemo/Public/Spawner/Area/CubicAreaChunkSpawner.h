#pragma once
#include "CoreMinimal.h"
#include "DespawnChunkSpawnerBase.h"
#include "CubicAreaChunkSpawner.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API ACubicAreaChunkSpawner : public ADespawnChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void GenerateArea() override;
};
