#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
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

	virtual FVoxel GetBorderVoxel(FVirtualMeshBuilder& IndexParams, int X, int Y, int Z) override;

PRIVATE_MODIFIER:
	void TraverseYDirection(FVirtualMeshBuilder& IndexParams, uint32 X, uint32 Y, uint32 Z,
	                        TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	                        TStaticArray<FVirtualMeshBuilder, CHUNK_FACE_COUNT>& BorderIndexParams, bool ShowBorders);

	static void CreateFace(
		const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
		const FStaticMergeData& StaticData,
		const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
		const int YEnd, const bool CanGenerate);

	void FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits,
	                    TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
	                    TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
	                    TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers, bool ShowBorders);

	void AdvanceAllMeshingEvents(FVirtualMeshBuilder& IndexParams, int X, int Y, int Z);

	// return true when interval advanced
	static bool AdvanceMeshingEvent(FVirtualMeshBuilder& IndexParams, const EMeshingEventIndex IntervalFlagIndex);

	static void CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
	                           const FStaticMergeData& StaticData,
	                           const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                           const int YEnd);

	void CreateVirtualVoxelFacesInLShape(FVirtualMeshBuilder& IndexParams,
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	TStaticArray<FVirtualMeshBuilder, CHUNK_FACE_COUNT>& BorderIndexParams, int X, int Y, int Z, bool ShowBorders);
	
	TSharedPtr<TArray<FRLEVoxel>> InitializeEdit(FVirtualMeshBuilder& IndexParams);
	void EditVoxelGrid(FVirtualMeshBuilder& IndexParams);

	void AdvanceEditInterval(FVirtualMeshBuilder& IndexParams) const;
	
	void CreateBorder(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		TStaticArray<FVirtualMeshBuilder, CHUNK_FACE_COUNT>& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& CurrentVoxelSample,
		int IndexInBetweenIntervals, EFaceDirection Direction,
		const int BorderX, const int BorderY, const int BorderZ, bool BorderCondition, bool ShowBorders);

	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};
