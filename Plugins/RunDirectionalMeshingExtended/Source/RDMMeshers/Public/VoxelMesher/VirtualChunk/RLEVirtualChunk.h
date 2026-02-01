#pragma once
#include "CoreMinimal.h"
#include "VirtualChunk.h"
#include "Voxel/RLEVoxel.h"
#include "RLEVirtualChunk.generated.h"

struct FVoxelEventMesher;
class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLEVirtualChunk : public UVirtualChunk
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FVoxelMesh& MeshContainer, FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelChanges) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual FVoxel GetBorderVoxel(FBorderEventMesher& BorderMeshingEventPlanner, const FIntVector& BorderVoxelPosition) override;

PRIVATE_MODIFIER:
	FCriticalSection MesherCriticalSection;
	FCriticalSection GridCriticalSection;
	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
	
	static TArray<TSharedPtr<FVoxelEventMesher>> UnusedMeshersPool;
};
