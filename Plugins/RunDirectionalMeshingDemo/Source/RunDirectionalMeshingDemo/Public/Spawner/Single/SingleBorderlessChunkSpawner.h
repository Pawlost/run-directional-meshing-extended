#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "SingleBorderlessChunkSpawner.generated.h"

struct FVoxelChange;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGDEMO_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing(FVoxelChange* VoxelChange) override;
	
private:
	TSharedPtr<FChunk> SideChunk[CHUNK_FACE_COUNT];
	void SpawnSideChunk(FMesherVariables& MeshVars, const FFaceToDirection& FaceDirection);
};
