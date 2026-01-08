#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "MeshingUtils/VoxelMeshContainer.h"
#include "Voxel/RLEVoxel.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
	                          TArray<FRLEVoxelEdit>& VoxelChanges, bool ShowBorders) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual FVoxel GetBorderVoxel(FBorderVirtualMeshEventPlanner& BorderMeshingEventPlanner, const FIntVector& BorderVoxelPosition) override;

PRIVATE_MODIFIER:

	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
	
	static TQueue<TSharedPtr<FVirtualMeshEventPlanner>, EQueueMode::Mpsc> UnusedMeshersPool;
};
