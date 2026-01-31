#pragma once
#include "CoreMinimal.h"
#include "VirtualChunk.h"
#include "Voxel/Voxel.h"
#include "BasicVirtualChunk.generated.h"

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API UBasicVirtualChunk : public UVirtualChunk
{
	GENERATED_BODY()

public:
	virtual FVoxel GetBorderVoxel(FBorderEventMesher& IndexParams, const FIntVector& BorderVoxelPosition) override;
	virtual void CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid) override;
	
	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelChanges) override;

private:
	FCriticalSection CriticalSection;
	FCriticalSection MesherCriticalSection;
	TSharedPtr<TArray<FVoxel>> BasicVoxelGridPtr;
};