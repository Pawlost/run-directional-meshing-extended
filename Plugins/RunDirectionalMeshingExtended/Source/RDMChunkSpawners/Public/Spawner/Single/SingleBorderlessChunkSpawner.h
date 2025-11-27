#pragma once
#include "CoreMinimal.h"
#include "FaceToDirection.h"
#include "SingleChunkSpawnerBase.h"
#include "VoxelMesher/MeshingUtils/VoxelEdit.h"
#include "SingleBorderlessChunkSpawner.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RDMCHUNKSPAWNERS_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing(TArray<FVoxelEdit>& VoxelChanges) override;
	
private:
	TSharedPtr<FChunk> SideChunk[CHUNK_FACE_COUNT];
	void SpawnSideChunk(FMesherVariables& MeshVars, const FFaceToDirection& FaceDirection);
};
