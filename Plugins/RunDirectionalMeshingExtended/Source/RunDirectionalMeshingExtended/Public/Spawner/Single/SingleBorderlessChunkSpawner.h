#pragma once
#include "CoreMinimal.h"
#include "SingleChunkSpawnerBase.h"
#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "SingleBorderlessChunkSpawner.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RUNDIRECTIONALMESHINGEXTENDED_API ASingleBorderlessChunkSpawner : public ASingleChunkSpawnerBase
{
	GENERATED_BODY()
	
protected:
	virtual void StartMeshing(TArray<FVoxelChange>& VoxelChanges) override;
	
private:
	TSharedPtr<FChunk> SideChunk[CHUNK_FACE_COUNT];
	void SpawnSideChunk(FMesherVariables& MeshVars, const FFaceToDirection& FaceDirection);
};
