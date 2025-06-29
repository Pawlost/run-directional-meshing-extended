#pragma once
#include "CoreMinimal.h"
#include "AreaChunkSpawnerBase.h"
#include "PreloadedVoxelCenterAreaChunkSpawner.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API APreloadedVoxelCenterAreaChunkSpawner : public AAreaChunkSpawnerBase
{
	GENERATED_BODY()

protected:
	virtual void GenerateArea() override;

private:
	virtual void BeginPlay() override;
};
