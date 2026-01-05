#pragma once
#include "RLEVoxelEdit.h"
#include "StaticMergeData.h"
#include "VirtualVoxelFaceContainer.h"
#include "Voxel/RLEVoxel.h"

class UVoxelMesherBase;

struct FMeshingEvent
{
	// Voxel sequence (run) to be traversed
	// If null the end is a chunk dimension, not end of sequence
	TSharedPtr<TArray<FRLEVoxel>> VoxelGridPtr;

	// Index where event ends
	uint32 LastEventIndex = 0;

	// Index of an run in a voxel array
	int32 VoxelRunIndex = 0;

	FORCEINLINE int GetEventIndex() const
	{
		return LastEventIndex + (*VoxelGridPtr)[VoxelRunIndex].RunLenght;
	}

	FORCEINLINE void AdvanceEvent()
	{
		LastEventIndex = LastEventIndex + GetCurrentVoxel().RunLenght;
		VoxelRunIndex++;
	}

	FORCEINLINE FRLEVoxel& GetCurrentVoxel() const
	{
		return (*VoxelGridPtr)[VoxelRunIndex];
	}
};

constexpr static int EventIndexCount = 5;

enum EMeshingEventIndex
{
	LeadingInterval = 0,
	FollowingXInterval = 1,
	FollowingZInterval = 2,
	EditEvent = 3,
	CopyEvent = 4
};

struct FRLEMeshingData
{
	const FStaticMergeData FaceData;
	EMeshingEventIndex MeshingEventIndex;
};
	
class SimpleVirtualVoxelFace
{
	
};

/*
Front = 0,
Back = 1,
Right = 2,
Left = 3,
Bottom = 4,
Top = 5
*/
class VirtualMeshEventPlanner
{
public:
	VirtualMeshEventPlanner(){}
	
	VirtualMeshEventPlanner(const uint32 VoxelLine,
	const uint32 VoxelPlane, const uint32 MaxNumberOfVoxels, 
	TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT>& VirtualFaces,
	TStaticArray<FVirtualVoxelFaceContainer, CHUNK_FACE_COUNT>& SideFaces, bool ShowBorders);
	
	void AdvanceEditInterval(uint32 MaxNumberOfVoxel, TArray<FRLEVoxelEdit>& VoxelEdits);
	void TryUpdateNextMeshingEvent(const uint32 EventIndex);
	
	void InitializeIntervals(TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid, TArray<FRLEVoxelEdit>& VoxelEdits);
	
	void GenerateVirtualFaces();
	
	FORCEINLINE bool IsEditEnabled() const
	{
		return EditEnabled;
	}
	
	TSharedPtr<TArray<FRLEVoxel>> GetMainVoxelGridPtr() const
	{
		return MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	}
	
	void CreateVirtualVoxelFacesInLShape(
TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams);
	
	void TraverseYDirection(TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams);

	void CreateFace(
	const TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT>& VirtualFaces,
	const int FaceIndex, const FRLEVoxel& RLEVoxel, const int YEnd, const bool CanGenerate, int VoxelPlanePosition);

	void AdvanceAllMeshingEvents();
	
	// return true when interval advanced
	bool AdvanceMeshingEvent(const EMeshingEventIndex IntervalFlagIndex);
	
	void EditVoxelGrid();
	
	void CreateBorder(
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams,
		 const FRLEVoxel& CurrentVoxelSample,
		EFaceDirection Direction, FIntVector BorderPosition, bool BorderCondition);

private:
	bool ShowBorders = false;
	
	// Current event index made of all meshing events that were already processed/traversed.
	uint32 CurrentMeshingEventIndex = 0;

	// After reaching closest end, updates it and sets next voxel interval to next
	// End is equivalent to event in Discrete Event Simulation 
		
	FMeshingEvent MeshingEvents[EventIndexCount];
	uint32 NextMeshingEventIndex = 0;
	uint32 IndexSequenceBetweenEvents = 0;
	FRLEVoxel* PreviousVoxelRun = nullptr;
	
	FIntVector<uint32> CurrentVoxelPosition = FIntVector(0, 0, 0);

	uint32 ContinueEditIndex = 0;
	bool EditEnabled = false;
	FIntVector PreviousPosition = FIntVector(0, 0, 0);
		
	TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT> VirtualFaces;
	TStaticArray<FVirtualVoxelFaceContainer, CHUNK_FACE_COUNT> SideFaces;
	
	const uint32 VoxelLine = 0;
	const uint32 VoxelPlane = 0;
	const uint32 MaxNumberOfVoxels = 0;
};
	