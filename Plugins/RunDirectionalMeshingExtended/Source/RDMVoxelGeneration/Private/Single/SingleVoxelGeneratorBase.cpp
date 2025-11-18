#include "Single/SingleVoxelGeneratorBase.h"

void USingleVoxelGeneratorBase::BeginPlay()
{
	checkf(!VoxelTypeHandle.IsNull(), TEXT("Table row with voxel type must by set"))
	Super::BeginPlay();
}

FVoxel USingleVoxelGeneratorBase::GetSingleVoxel() const
{
	return GetVoxelByName(VoxelTypeHandle.RowName);
}

TTuple<FName, FVoxelTableRow> USingleVoxelGeneratorBase::GetVoxelTableRow(const FVoxel& Voxel) const
{
	// Explore voxel table attached to row handle
	const auto VoxelTable = VoxelTypeHandle.DataTable;
	auto RowNames = VoxelTable->GetRowNames();
	auto RowIndex = Voxel.GetRowIndex();
	checkf(RowNames.IsValidIndex(RowIndex), TEXT("Voxel Id out of bounds"));
	// Voxel index should be created from the same table
	auto RowName = RowNames[RowIndex];
	return TTuple<FName, FVoxelTableRow>(
		RowName, *VoxelTable->FindRow<FVoxelTableRow>(RowName, "Could not find voxel id in row handle"));
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
			const auto VoxelType = VoxelTypeHandle.DataTable->FindRow<FVoxelTableRow>(
				VoxelTypeHandle.RowName, "Could not find voxel name in row handle");
			
			return FVoxel(Index, VoxelType != nullptr && VoxelType->bIsTransparent);
		}
	}
	return FVoxel();
}