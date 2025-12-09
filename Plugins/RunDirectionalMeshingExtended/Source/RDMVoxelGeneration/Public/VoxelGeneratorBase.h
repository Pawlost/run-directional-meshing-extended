#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "VoxelTableRow.h"
#include "Chunk/VirtualVoxelChunk.h"
#include "Voxel/Voxel.h"
#include "VoxelGeneratorBase.generated.h"

struct FMesherVariables;
class UVoxelMesherBase;

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

	void AddVoxelsToChunk(const FVirtualVoxelChunk& Chunk);
	
protected:
	virtual void GenerateVoxels(const FVirtualVoxelChunk& Chunk, TArray<FVoxel>& VoxelModel) 
		PURE_VIRTUAL(UVoxelGeneratorBase::GenerateVoxels);

private:
	FCriticalSection Mutex;
};
