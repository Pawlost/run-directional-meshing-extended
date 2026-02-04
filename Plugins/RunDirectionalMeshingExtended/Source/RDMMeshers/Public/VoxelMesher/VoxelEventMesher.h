#pragma once
#include "MeshingUtil/MeshingEvent.h"
#include "VoxelEventMesherBase.h"
#include "MeshingUtil/RLEVoxelEdit.h"
#include "MeshingUtil/VirtualVoxelFaceContext.h"
#include "MeshingUtil/VoxelMesh.h"
#include "Voxel/RLEVoxel.h"

struct FChunkBorderContext;
/*
Front = 0,
Back = 1,
Right = 2,
Left = 3,
Bottom = 4,
Top = 5
*/
struct FVoxelEventMesher : FVoxelEventMesherBase
{
public:
	FVoxelEventMesher(const TObjectPtr<UBaseVoxelData> BaseVoxelData);
	
	void UpdateInternalState(const uint32 VoxelLineParam, const uint32 VoxelPlaneParam,
	const uint32 MaxVoxelsInChunkParam);
	
	void AdvanceEditInterval(TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void InitializeIntervals(const TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void GenerateVirtualFaces(FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	FORCEINLINE bool IsEditEnabled() const
	{
		return EditEnabled;
	}
	
	FORCEINLINE TSharedPtr<TArray<FRLEVoxel>> GetMainVoxelGridPtr() const
	{
		return MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	}
	
	void CreateVirtualVoxelFacesInLShape(FChunkBorderContext& BorderParameters);
	
	void TraverseYDirection(FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void AdvanceAllMeshingEvents();
	
	void ConvertVirtualFacesToMesh(FVoxelMesh& VoxelMeshContainer, const double VoxelSize);
	
	void EditVoxelGrid(TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void CreateBorder(FChunkBorderContext& BorderContext, uint32 YEnd,
		 const FRLEVoxel& CurrentVoxelSample, EFaceDirection Direction, FIntVector BorderPosition, bool BorderCondition);
	
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
		
	TArray<FVirtualVoxelFaceContext> VirtualFaces;
};
	