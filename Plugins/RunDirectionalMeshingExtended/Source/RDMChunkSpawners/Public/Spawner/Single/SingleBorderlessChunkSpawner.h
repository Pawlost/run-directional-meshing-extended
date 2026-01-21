#pragma once
#include "CoreMinimal.h"
#include "FaceToDirection.h"
#include "SingleChunkSpawnerBase.h"
#include "SingleBorderlessChunkSpawner.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RDMCHUNKSPAWNERS_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing(TArray<FRLEVoxelEdit>& VoxelChanges) override;
	
private:
	TSharedPtr<FChunk> SideChunk[VOXEL_FACE_COUNT];
	void SpawnSideChunk(const FFaceToDirection& FaceDirection);
};
