#pragma once
#include "BorderVirtualMeshEventPlanner.h"
#include "MeshingEvent.h"
#include "VirtualMeshEventPlannerBase.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelMesher/MeshingUtils/BorderVisualizationOption.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"
#include "VoxelMesher/MeshingUtils/RLEVoxelEdit.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFaceContainer.h"
#include "VoxelMesher/MeshingUtils/VoxelMeshContainer.h"

struct FVirtualMeshEventPlanner;
class UVirtualChunk;

struct FBorderParams
{
	TStaticArray<FBorderVirtualMeshEventPlanner, CHUNK_FACE_COUNT> BorderIndexParams;
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, CHUNK_FACE_COUNT> SideMeshers;
};

/*
Front = 0,
Back = 1,
Right = 2,
Left = 3,
Bottom = 4,
Top = 5
*/
struct FVirtualMeshEventPlanner : FVirtualMeshEventPlannerBase
{
public:
	FVirtualMeshEventPlanner(const uint32 VoxelLine,
		const uint32 VoxelPlane, const uint32 MaxNumberOfVoxels);
	
	void UpdateInternalState(const EBorderVisualizationOption BorderVisualization, const uint32 VoxelLineParam, const uint32 VoxelPlaneParam,
	const uint32 MaxVoxelsInChunkParam);
	
	void AdvanceEditInterval(TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void InitializeIntervals(TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void GenerateVirtualFaces(FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	FORCEINLINE bool IsEditEnabled() const
	{
		return EditEnabled;
	}
	
	FORCEINLINE TSharedPtr<TArray<FRLEVoxel>> GetMainVoxelGridPtr() const
	{
		return MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	}
	
	void CreateVirtualVoxelFacesInLShape(FBorderParams& BorderParameters);
	
	void TraverseYDirection(FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits);

	void CreateFace(const EFaceDirection FaceIndex, const FRLEVoxel& RLEVoxel, const int YEnd, const bool CanGenerate, int VoxelPlanePosition);

	void AdvanceAllMeshingEvents();
	
	void DirectionalGreedyMerge(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize);
	
	void EditVoxelGrid(TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void CreateBorder(FBorderParams& BorderParameters,
		FIntVector VoxelPosition, uint32 YEnd,
		 const FRLEVoxel& CurrentVoxelSample,
		EFaceDirection Direction,
		FIntVector SideChunkBorderPosition, bool BorderCondition);
	
	void InternalReset();

private:

	// After reaching closest end, updates it and sets next voxel interval to next
	// End is equivalent to event in Discrete Event Simulation 
		
	FMeshingEvent MeshingEvents[EventIndexCount];
	
	uint32 IndexSequenceBetweenEvents = 0;
	FRLEVoxel* PreviousVoxelRun = nullptr;
	FIntVector CurrentVoxelPosition = FIntVector(0, 0, 0);
	uint32 ContinueEditIndex = 0;
	bool EditEnabled = false;
	FIntVector PreviousPosition = FIntVector(0, 0, 0);
		
	TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT> VirtualFaces;
	
	uint32 VoxelLine = 0;
	uint32 VoxelPlane = 0;
	uint32 MaxVoxelsInChunk = 0;
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
	
	TArray<FVirtualVoxelFace> FirstDirectionalMeshingHelperArray;
	TArray<FVirtualVoxelFace> SecondDirectionalMeshingHelperArray;
};
	