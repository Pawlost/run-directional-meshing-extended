#pragma once

#include "CoreMinimal.h"
#include "VoxelMesher/MeshingUtils/VoxelEdit.h"
#include "ChunkEdit.generated.h"

USTRUCT(BlueprintType)
struct RDMVOXELGENERATION_API FChunkEdit
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVoxelEdit> VoxelEdits;
};


