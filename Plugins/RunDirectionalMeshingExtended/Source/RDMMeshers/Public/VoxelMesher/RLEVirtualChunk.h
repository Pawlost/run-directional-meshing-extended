#pragma once
#include "CoreMinimal.h"
#include "VirtualChunk.h"
#include "MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "MeshingUtils/VoxelMeshContainer.h"
#include "Voxel/RLEVoxel.h"
#include "RLEVirtualChunk.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLEVirtualChunk : public UVirtualChunk
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelChanges) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual FVoxel GetBorderVoxel(FBorderVirtualMeshEventPlanner& BorderMeshingEventPlanner, const FIntVector& BorderVoxelPosition) override;

PRIVATE_MODIFIER:
	FCriticalSection MesherCriticalSection;
	FCriticalSection GridCriticalSection;
	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
	
	static TArray<TSharedPtr<FVirtualMeshEventPlanner>> UnusedMeshersPool;
};
