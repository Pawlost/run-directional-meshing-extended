#pragma once
#include "CoreMinimal.h"
#include "VirtualChunkBase.h"
#include "Voxel/Voxel.h"
#include "BasicVirtualChunk.generated.h"

struct FChunkBorderContext;
struct FVoxelMesh;
struct FBorderEventMesher;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API UBasicVirtualChunk : public UVirtualChunkBase
{
	GENERATED_BODY()

public:
	virtual FVoxel GetBorderVoxel(FBorderEventMesher& IndexParams, const FIntVector& BorderVoxelPosition) override;
	virtual void CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid) override;

	virtual void GenerateMesh(FVoxelMesh& MeshContainer, FChunkBorderContext& BorderParameters,
	                          TArray<FRLEVoxelEdit>& VoxelChanges) override;

private:
	FCriticalSection CriticalSection;
	FCriticalSection MesherCriticalSection;
	TSharedPtr<TArray<FVoxel>> BasicVoxelGridPtr;
};
