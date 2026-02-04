#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "VoxelTableRow.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkActor.h"
#include "Voxel/Voxel.h"
#include "VoxelGeneratorBase.generated.h"

struct FMesherVariables;
class UVirtualChunk;

/**
 * Base for components used to fill voxel models with voxels.
 */
UCLASS(ClassGroup=(VoxelGeneration), Abstract, Blueprintable)
class RDMVOXELGENERATION_API UVoxelGeneratorBase : public UBaseVoxelData
{
	GENERATED_BODY()

public:
	
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelByName, return FVoxel();)
	
	virtual TTuple<FName, FVoxelTableRow> GetVoxelTableRow(const FVoxel& Voxel) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelTableRow, return TTuple<FName, FVoxelTableRow>();)

	void GenerateVoxels(FChunk& Chunk);
	
protected:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) 
		PURE_VIRTUAL(UVoxelGeneratorBase::GenerateVoxels);

	virtual void BeginPlay() override;

private:
	FCriticalSection Mutex;
};
