#pragma once
#include "CoreMinimal.h"
#include "DespawnChunkSpawnerBase.h"
#include "CenterAreaChunkSpawner.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API ACenterAreaChunkSpawner : public ADespawnChunkSpawnerBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  Category = "Chunk")
	int32 MeshZone = 2;
	
protected:
	virtual void GenerateArea() override;
	
private:
	const int LowerThreadLimit = 6;
};
