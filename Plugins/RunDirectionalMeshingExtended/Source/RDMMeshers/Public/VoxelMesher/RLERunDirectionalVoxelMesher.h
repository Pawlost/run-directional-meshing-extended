#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "MeshingUtils/VirtualMeshEventPlanner.h"
#include "Voxel/RLEVoxel.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, 6>& VirtualFaces,
								FVoxelMeshContainer& MeshContainer,
	                          TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	                          TArray<FRLEVoxelEdit>& VoxelChanges,
	                          TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
	                          TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers, bool ShowBorders) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual FVoxel GetBorderVoxel(FVirtualMeshEventPlanner& IndexParams, FIntVector VoxelPosition) override;

PRIVATE_MODIFIER:

	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
	
	static TQueue<TSharedPtr<FVirtualMeshEventPlanner>, EQueueMode::Mpsc> UnusedMeshersPool;
};
