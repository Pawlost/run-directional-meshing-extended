﻿#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
#include "MeshingUtils/ProcMeshSectionVars.h"
#include "MeshingUtils/MeshingDirections.h"
#include "RunDirectionalVoxelMesher.generated.h"

class UVoxelGrid;
class URLEVoxelGrid;
struct FChunkParams;

UCLASS(ClassGroup=(Meshers), Blueprintable)
class RUNDIRECTIONALMESHINGEXTENDED_API URunDirectionalVoxelMesher : public UVoxelMesherBase
{
	GENERATED_BODY()

public:
	virtual void GenerateMesh(FMesherVariables& MeshVars, FVoxelChange* VoxelChange) override;

private:
	struct FVoxelIndexParams
	{
		bool IsBorder;
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		int32 CurrentVoxelIndex;
		FVoxel CurrentVoxel;
		EFaceDirection FaceDirection;
	};

	static bool IsBorderVoxelVisible(const FVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct);
	static bool IsVoxelVisible(const UVoxelGrid& VoxelGridObject, const FVoxelIndexParams& FaceData);

	void IncrementRun(int X, int Y, int Z, int32 AxisVoxelIndex, bool bIsMinBorder, bool bIsMaxBorder,
	                  const FMeshingDirections& FaceTemplate, const FMeshingDirections& ReversedFaceTemplate,
	                  const FMesherVariables& MeshVars, const UVoxelGrid& VoxelGridObject) const;

	static void AddFace(const UVoxelGrid& VoxelGridObject, const FMeshingDirections& FaceTemplate, bool bIsBorder,
	                    const int32& Index, const FIntVector& Position, const FVoxel& Voxel,
	                    const int32& AxisVoxelIndex,
	                    const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces, const FChunkParams& ChunkParams);

	void FaceGeneration(const UVoxelGrid& VoxelGridObject, FMesherVariables& MeshVars) const;

	static void ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray, const FVoxelFace& Face,
	                                  TMap<uint32, uint32>& LocalVoxelTable, int FaceIndex, double VoxelSize);

	void ChangeVoxelId(const UVoxelGrid& VoxelGridObject, TMap<int32, uint32>& VoxelTable,
	                   const FVoxelChange& VoxelChange) const;

	void DirectionalGreedyMerge(const FMesherVariables& MeshVars, const TSharedPtr<TArray<FProcMeshSectionVars>>& QuadMeshSectionArray,
	                                                               TMap<uint32, uint32>& LocalVoxelTable, const double VoxelSize,
	                                                               const FStaticGreedyMergeData& GreedyMergeData, int Coord1, int Coord2) const;

	void GenerateProcMesh(const FMesherVariables& MeshVars, TMap<uint32, uint32> LocalVoxelTable,
		TSharedPtr<TArray<FProcMeshSectionVars>>& QuadMeshSectionArray) const;
};
