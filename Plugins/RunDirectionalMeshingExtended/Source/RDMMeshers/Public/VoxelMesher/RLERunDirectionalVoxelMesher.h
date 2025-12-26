#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"
#include "RLERunDirectionalVoxelMesher.generated.h"

class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RDMMESHERS_API URLERunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
	                          TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, 6>& VirtualFaces,
	                          TMap<int32, uint32>& LocalVoxelTable,
	                          TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	                          TArray<FRLEVoxelEdit>& VoxelChanges,
	                          TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
	                          TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers,
	                          FBorderSamples& BorderSamples) override;

	virtual void CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual bool IsBorderVoxelEmpty(FIndexParams& IndexParams, int X, int Y, int Z) override;

PRIVATE_MODIFIER:
	void TraverseYDirection(FIndexParams& IndexParams, uint32 X, uint32 Y, uint32 Z,
	                        TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	                        TStaticArray<FIndexParams, CHUNK_FACE_COUNT>& BorderIndexParams);

	static void CreateFace(
		const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
		const FStaticMergeData& StaticData,
		const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
		const int YEnd, const bool CanGenerate);

	void FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits,
	                    TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces,
	                    TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
	                    TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers);

	void AdvanceAllMeshingEvents(FIndexParams& IndexParams, int X, int Y, int Z);

	// return true when interval advanced
	static bool AdvanceMeshingEvent(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex);

	static void CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
	                           const FStaticMergeData& StaticData,
	                           const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                           const int YEnd);

	void CreateVirtualVoxelFacesInLShape(FIndexParams& IndexParams,
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	TStaticArray<FIndexParams, CHUNK_FACE_COUNT>& BorderIndexParams,	
	int X, int Y, int Z);
	
	void InitializeEdit(FIndexParams& IndexParams);
	void EditVoxelGrid(FIndexParams& IndexParams);

	void AdvanceEditInterval(FIndexParams& IndexParams) const;


	FORCEINLINE void CreateBorder(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		TStaticArray<FIndexParams, CHUNK_FACE_COUNT>& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample,
		int IndexInBetweenIntervals, EFaceDirection Direction,
		const int BorderX, const int BorderY, const int BorderZ, bool BorderCondition);

	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};
