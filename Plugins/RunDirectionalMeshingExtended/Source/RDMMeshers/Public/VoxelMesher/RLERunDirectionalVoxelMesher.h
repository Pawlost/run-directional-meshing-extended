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
	                          TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, 6>& SideMeshers, FBorderSamples& BorderSamples) override;

	virtual TStrongObjectPtr<UVoxelModel> CompressVoxelModel(TArray<FVoxel>& VoxelGrid) override;

	virtual bool SampleChunkBorder(FIndexParams& IndexParams, int X, int Y, int Z) override;
	
	void AddLeftBorderSample(FBorderSamples& SampledBorderChunks, 
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	
	void AddRightBorderSample(FBorderSamples& SampledBorderChunks, 
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	
	void AddTopBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddBottomBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddFrontBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	void AddBackBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const;
	
	
	virtual void BorderGeneration(const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
							TMap<int32, uint32>& BorderLocalVoxelTable, 
						   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
							TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples, 
							EFaceDirection FaceDirection) override;
	
	
PRIVATE_MODIFIER:

	// 8 = interval combinations/types
	TStaticArray<TArray<FRLEMeshingData>, 8> MeshingData;

	static void CreateFace(const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
											   const FStaticMergeData& StaticData,
											   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
											   const int YEnd, const bool CanGenerate);

	void FaceGeneration(FIndexParams& IndexParams, TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers);
	void AdvanceAllMeshingEvents(FIndexParams& IndexParams, int X, int Y, int Z);

	// return true when interval advanced
	static bool AdvanceMeshingEvent(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex);

	static void CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
	                           const FStaticMergeData& StaticData,
	                           const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
	                           const int YEnd);
	
	static void SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index);

	void AddBorderFace(TArray<FVirtualVoxelFace>& FaceContainer,
												   TArray<FVirtualVoxelFace>& InverseFaceContainer,
												   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
													TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples,
												   const FMeshingDirections& FaceTemplate,
												   const FMeshingDirections& InverseFaceTemplate, int X, int Y) const;

	void CreateVirtualVoxelFacesInLShape(FIndexParams& IndexParams, int X, int Y, int Z);
	void InitializeEdit(FIndexParams& IndexParams);
	void EditVoxelGrid(FIndexParams& IndexParams);

	void AdvanceEditInterval(FIndexParams& IndexParams) const;
	
	TSharedPtr<TArray<FRLEVoxel>> RLEVoxelGrid;
};
