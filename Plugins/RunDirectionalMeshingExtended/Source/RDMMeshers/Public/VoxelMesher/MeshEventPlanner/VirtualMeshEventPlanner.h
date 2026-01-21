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
	TStaticArray<FBorderVirtualMeshEventPlanner, VOXEL_FACE_COUNT> BorderIndexParams;
	TStaticArray<TStrongObjectPtr<UVirtualChunk>, VOXEL_FACE_COUNT> SideMeshers;
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
	
	void AdvanceAllMeshingEvents();
	
	void ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize);
	
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
	uint32 ContinueEditIndex = 0;
	bool EditEnabled = false;
	FIntVector PreviousPosition = FIntVector(0, 0, 0);
		
	TArray<FVirtualVoxelFaceContainer> VirtualFaces;
	EBorderVisualizationOption BorderVisualization = EBorderVisualizationOption::None;
};
	