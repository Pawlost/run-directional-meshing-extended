#pragma once
#include "Voxel/RLEVoxel.h"
#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

constexpr static int EventIndexCount = 5;

enum EMeshingEventIndex
{
	LeadingInterval = 0,
	FollowingXInterval = 1,
	FollowingZInterval = 2,
	EditEvent = 3,
	CopyEvent = 4
};

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
		LastEventIndex = LastEventIndex + GetCurrentRLEVoxel().RunLenght;
		VoxelRunIndex++;
	}

	FORCEINLINE FRLEVoxel& GetCurrentRLEVoxel() const
	{
		return (*VoxelGridPtr)[VoxelRunIndex];
	}
	
	FORCEINLINE FVoxel& GetCurrentVoxel() const
	{
		return GetCurrentRLEVoxel().Voxel;
	}
};
