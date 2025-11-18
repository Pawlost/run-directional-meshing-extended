#pragma once
#include "CoreMinimal.h"
#include "BaseVoxelData.h"
#include "VoxelTableRow.h"
#include "Chunk/Chunk.h"
#include "Chunk/ChunkActor.h"
#include "Voxel/Voxel.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"
#include "VoxelGeneratorBase.generated.h"

struct FVoxelEdit;
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
	UPROPERTY(EditAnywhere, Category ="Voxels")
	bool bEnableVoxelMeshing = true;

	void GenerateMesh(FMesherVariables& MesherVariables, TArray<FVoxelEdit>& VoxelChanges) const;
	
	virtual FVoxel GetVoxelByName(const FName& VoxelName) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelByName, return FVoxel();)
	
	virtual TTuple<FName, FVoxelTableRow> GetVoxelTableRow(const FVoxel& Voxel) const PURE_VIRTUAL(
		UVoxelGeneratorBase::GetVoxelTableRow, return TTuple<FName, FVoxelTableRow>();)

	void GenerateVoxels(FChunk& Chunk);
	
	void GenerateProcMesh(const FMesherVariables& MeshVars) const;
	
	void AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor, TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData,
		const TMap<int32, uint32>& LocalVoxelTable) const;
	
protected:
	virtual void AddVoxels(FChunk& Chunk, TArray<FVoxel>& VoxelModel) 
		PURE_VIRTUAL(UVoxelGeneratorBase::GenerateVoxels);
};
