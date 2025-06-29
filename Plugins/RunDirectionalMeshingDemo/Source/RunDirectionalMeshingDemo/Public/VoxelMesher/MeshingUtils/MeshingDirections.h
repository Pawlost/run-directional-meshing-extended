#pragma once
#include "StaticMergeData.h"

struct FMeshingDirections
{
	const FStaticMergeData& StaticMeshingData;
	int32 ForwardVoxelIndex;
	int32 ChunkBorderIndex;
	int32 PreviousVoxelIndex;

	explicit FMeshingDirections(FStaticMergeData& StaticMeshingData) : StaticMeshingData(StaticMeshingData),
	                                                                         ForwardVoxelIndex(0), ChunkBorderIndex(0),
	                                                                         PreviousVoxelIndex(0)
	{
	}
};
