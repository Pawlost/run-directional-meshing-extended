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
	                          TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>>& LocalVoxelTable,
	                          TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	                          TArray<FRLEVoxelEdit>& VoxelChanges,
	                          TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
	                          TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers, bool ShowBorders) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual FVoxel GetBorderVoxel(VirtualMeshEventPlanner& IndexParams, FIntVector VoxelPosition) override;

PRIVATE_MODIFIER:

	void FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits,
	                    TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
	                    TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
	                    TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers, bool ShowBorders);

	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};
