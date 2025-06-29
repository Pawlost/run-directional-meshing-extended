#include "Voxel/Generator/Single/SingleVoxelGeneratorBase.h"

void USingleVoxelGeneratorBase::BeginPlay()
{
	checkf(!VoxelTypeHandle.IsNull(), TEXT("Table row with voxel type must by set"))
	Super::BeginPlay();
}

FVoxel USingleVoxelGeneratorBase::GetSingleVoxel() const
{
	return GetVoxelByName(VoxelTypeHandle.RowName);
}

TTuple<FName, FVoxelType> USingleVoxelGeneratorBase::GetVoxelTypeById(const int32& VoxelId) const
{
	// Explore voxel table attached to row handle
	const auto VoxelTable = VoxelTypeHandle.DataTable;
	auto RowNames = VoxelTable->GetRowNames();
	checkf(RowNames.IsValidIndex(VoxelId), TEXT("Voxel Id out of bounds"));
	// Voxel index should be created from the same table
	auto RowName = RowNames[VoxelId];
	return TTuple<FName, FVoxelType>(
		RowName, *VoxelTable->FindRow<FVoxelType>(RowName, "Could not find voxel id in row handle"));
}

FVoxel USingleVoxelGeneratorBase::GetVoxelByName(const FName& VoxelName) const
{
	// Explore voxel table attached to row handle
	TArray<FName> RowNames = VoxelTypeHandle.DataTable->GetRowNames();
	for (int32 Index = 0; Index < RowNames.Num(); Index++)
	{
		// Search row name
		if (RowNames[Index] == VoxelName)
		{
			const auto VoxelType = VoxelTypeHandle.DataTable->FindRow<FVoxelType>(
				VoxelTypeHandle.RowName, "Could not find voxel name in row handle");
			return FVoxel(Index, VoxelType->bIsTransparent);
		}
	}
	return FVoxel();
}
