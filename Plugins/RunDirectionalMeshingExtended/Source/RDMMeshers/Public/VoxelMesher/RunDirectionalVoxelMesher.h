#pragma once
#include "CoreMinimal.h"
#include "VirtualChunk.h"
#include "RunDirectionalVoxelMesher.generated.h"

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URunDirectionalVoxelMesher : public UVirtualChunk
{
	GENERATED_BODY()

public:
	virtual FVoxel GetBorderVoxel(FBorderVirtualMeshEventPlanner& IndexParams, const FIntVector& BorderVoxelPosition) override;
	virtual void CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid) override;
	
	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
						  TArray<FRLEVoxelEdit>& VoxelChanges,
						  EBorderVisualizationOption BorderVisualization) override;

	// void CheckBorderX(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
	// void CheckBorderY(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
	// void CheckBorderZ(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
	//
	FCriticalSection CriticalSection;
	TSharedPtr<TArray<FVoxel>> VoxelGridPtr;
};