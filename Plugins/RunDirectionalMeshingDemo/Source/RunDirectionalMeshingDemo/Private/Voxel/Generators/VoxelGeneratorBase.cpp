#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UVoxelGeneratorBase::BeginPlay()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	ChunkSize = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;

	Super::BeginPlay();

	checkf(VoxelMesherBlueprint, TEXT("Mesher blueprint must be set"));
	if (VoxelMesherBlueprint)
	{
		// Register mesher
		VoxelMesher = NewObject<UVoxelMesherBase>(this, VoxelMesherBlueprint);

		if (VoxelMesher)
		{
			VoxelMesher->SetVoxelGenerator(this);
			VoxelMesher->RegisterComponent();
		}
	}
}

void UVoxelGeneratorBase::ChangeKnownVoxelAtIndex(TArray<FVoxel>& VoxelGrid, TMap<int32, uint32>& VoxelTable,
                                                  const uint32& Index,
                                                  const FVoxel& Voxel)
{
	// NOTICE: Code here is optimized because voxel grid generation is not main topic of this bachelor's thesis 
	FScopeLock Lock(&Mutex);
	const auto PrevVoxel = VoxelGrid[Index];
	RemoveVoxelFromChunkTable(VoxelTable, PrevVoxel);

	// Replace the voxel
	VoxelGrid[Index] = Voxel;

	const auto VoxelTypeAmountInChunk = VoxelTable.Find(Voxel.VoxelId);
	if (VoxelTypeAmountInChunk != nullptr)
	{
		//Increase amount of added voxel
		(*VoxelTypeAmountInChunk) += 1;
	}
	else
	{
		//If voxel is unique, start new count
		VoxelTable.Add(Voxel.VoxelId, 1);
	}
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

double UVoxelGeneratorBase::GetChunkAxisSize() const
{
	return ChunkSize;
}

double UVoxelGeneratorBase::GetVoxelSize() const
{
	return VoxelSize;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunkDimension() const
{
	return VoxelCountPerChunkDimension;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunkLayer() const
{
	return VoxelCountYZ;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunk() const
{
	return VoxelCountXYZ;
}

void UVoxelGeneratorBase::GenerateMesh(FMesherVariables& MesherVariables, FVoxelChange* VoxelChange) const
{
	if (bEnableVoxelMeshing)
	{
		VoxelMesher->GenerateMesh(MesherVariables, VoxelChange);
	}
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkAxisSize();
}

void UVoxelGeneratorBase::RemoveVoxelFromChunkTable(TMap<int32, uint32>& VoxelTable, const FVoxel& Voxel)
{
	const auto VoxelTypeAmountInChunk = VoxelTable.Find(Voxel.VoxelId);
	if (VoxelTypeAmountInChunk != nullptr && !Voxel.IsEmptyVoxel())
	{
		auto& VoxelTypeAmountInChunkRef = *VoxelTypeAmountInChunk;
		// Reduce amount of previous voxel type because it will be replaced.
		VoxelTypeAmountInChunkRef--;

		// Amount is initialized with 1
		if (VoxelTypeAmountInChunkRef < 1)
		{
			// Remove id from a table if there is no voxel representation
			VoxelTable.Remove(Voxel.VoxelId);
		}
	}
}
