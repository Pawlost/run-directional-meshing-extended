#pragma once

#include "CoreMinimal.h"
#include "Voxel.generated.h"

/**
 * Represents a single voxel in the voxel grid.
 * Contains an ID and a transparency flag.
 */
USTRUCT()
struct RDMMESHERS_API FVoxel
{
	GENERATED_BODY()

	// Empty voxel ID tied to FVoxel namespace
	static constexpr int32 EMPTY_VOXEL = 0; 
	
	// Offset is important for transparent voxel IDs.
	static constexpr int32 OFFSET_FROM_EMPTY_VOXEL = FVoxel::EMPTY_VOXEL + 1;

	// Saving voxels is not implemented in this demo, but a property specifier for saving has been added.
	UPROPERTY(SaveGame)
	int32 VoxelId = EMPTY_VOXEL;

	FVoxel(){}

	explicit FVoxel(const int32 VoxelId)
	{
		this->VoxelId = VoxelId;
	}
	
	explicit FVoxel(const uint32 RowIndex, const bool IsTransparent = false)
	{
		const int32 OffsetId = RowIndex + OFFSET_FROM_EMPTY_VOXEL;
		VoxelId = IsTransparent ? -OffsetId : OffsetId;
	}

	FORCEINLINE uint32 GetRowIndex() const
	{
		return FMath::Abs(VoxelId) - OFFSET_FROM_EMPTY_VOXEL;
	}
	
	FORCEINLINE bool IsEmptyVoxel() const
	{
		return VoxelId == EMPTY_VOXEL;
	}

	FORCEINLINE bool IsTransparent() const
	{
		return VoxelId < EMPTY_VOXEL;
	}

	FORCEINLINE bool operator==(const FVoxel& OtherVoxel) const
	{
		// Transparent value is not considered during comparison because VoxelIds are the same.
		return VoxelId == OtherVoxel.VoxelId;
	}

	FORCEINLINE bool operator!=(const FVoxel& OtherVoxel) const
	{
		return VoxelId != OtherVoxel.VoxelId;
	}
};
