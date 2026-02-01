#pragma once

#include "CoreMinimal.h"
#include "MesherVariables.h"
#include "ProceduralMeshComponent.h"
#include "VoxelMesher/MeshingUtil/Enum/BorderVisualizationOption.h"
#include "ChunkActor.generated.h"

struct FRLEVoxelEdit;
class UVoxelGeneratorBase;

UCLASS()
class RDMVOXELGENERATION_API AChunkActor : public AActor
{
	GENERATED_BODY()

public:
	AChunkActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;
	
	FORCEINLINE bool HasMesh() const;
	
	void ClearMesh() const;
	void SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase);
		
	void GenerateMesh(FMesherVariables& MeshVars, TArray<FRLEVoxelEdit>& VoxelEdits, EBorderVisualizationOption BorderVisualization) const;
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	
	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;
	
	void AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor,
                        	const FVoxelMesh& LocalVoxelTable) const;
};
