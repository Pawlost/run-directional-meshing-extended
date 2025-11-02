#include "Voxel/Generator/VoxelGeneratorBase.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UVoxelGeneratorBase::BeginPlay()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	ChunkSpacing = VoxelCountY * VoxelSize;
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

void UVoxelGeneratorBase::ChangeKnownVoxelAtIndex(TArray<FVoxel>& VoxelGrid,
                                                  const uint32& Index,
                                                  const FVoxel& Voxel)
{
	// NOTICE: Code here is optimized because voxel grid generation is not main topic of this bachelor's thesis 
	FScopeLock Lock(&Mutex);
	// Replace the voxel
	VoxelGrid[Index] = Voxel;
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

double UVoxelGeneratorBase::GetChunkSpacing() const
{
	return ChunkSpacing;
}

double UVoxelGeneratorBase::GetVoxelSize() const
{
	return VoxelSize;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerVoxelLine() const
{
	return VoxelCountPerChunkDimension;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerVoxelPlane() const
{
	return VoxelCountYZ;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunk() const
{
	return VoxelCountXYZ;
}

void UVoxelGeneratorBase::GenerateMesh(FMesherVariables& MesherVariables, TArray<FVoxelEdit>& VoxelChanges) const
{
	if (bEnableVoxelMeshing)
	{
		VoxelMesher->GenerateMesh(MesherVariables, VoxelChanges);
	}
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkSpacing();
}