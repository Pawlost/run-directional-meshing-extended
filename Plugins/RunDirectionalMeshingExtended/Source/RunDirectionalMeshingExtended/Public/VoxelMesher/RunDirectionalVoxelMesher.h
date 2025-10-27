#pragma once
#include "CoreMinimal.h"
#include "VoxelMesherBase.h"
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
	virtual void GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelEdit>& VoxelChanges) override;

private:
	
	struct FBorderVoxelIndexParams
	{
		int32 SideChunkVoxelIndex;
		FStaticMergeData StaticData;
		FVoxelParams VoxelParams;
	};
	
	struct FInnerVoxelIndexParams
	{
		int32 ForwardVoxelIndex;
		int32 PreviousVoxelIndex;
		FVoxelParams VoxelParams;
	};

	static bool IsBorderVoxelVisible(const FBorderVoxelIndexParams& FaceData, const FChunkParams& ChunkStruct);
	static bool IsVoxelVisible(const UVoxelGrid& VoxelGridObject, const FInnerVoxelIndexParams& FaceData);

	void IncrementRun(int X, int Y, int Z, 
	                  const FMeshingDirections& FaceTemplate, const FMeshingDirections& ReversedFaceTemplate,
	                  const FMesherVariables& MeshVars, const UVoxelGrid& VoxelGridObject) const;

	static void CheckVoxelNeighborhood(const UVoxelGrid& VoxelGridObject, const FMeshingDirections& FaceTemplate,
						const int32& Index, const FVoxelParams& VoxelParams,
	                    const TSharedPtr<TArray<FVirtualVoxelFace>>& ChunkFaces);

	
	void FaceGeneration(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars) const;

	void ChangeVoxelId(const UVoxelGrid& VoxelGridObject, TMap<int32, uint32>& VoxelTable,
	                   const FVoxelEdit& VoxelChange) const;
	
	void IncrementBorderRun(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars,
	                        const FMeshingDirections& FaceTemplate, const FIntVector& Position, int BorderIndex) const;
	
	void CheckBorderX(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
	void CheckBorderY(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
	void CheckBorderZ(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, int Y, int Z) const;
};