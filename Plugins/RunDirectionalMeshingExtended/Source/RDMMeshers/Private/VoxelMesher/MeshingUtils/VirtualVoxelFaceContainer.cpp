#include "VoxelMesher/MeshingUtils//VirtualVoxelFaceContainer.h"

FVirtualVoxelFaceContainer::FVirtualVoxelFaceContainer()
{
	VirtualVoxelFaces = MakeShared<TArray<FVirtualVoxelFace>>();
}

FVirtualVoxelFaceContainer::FVirtualVoxelFaceContainer(uint32 VoxelPlane)
{
	VirtualVoxelFaces = MakeShared<TArray<FVirtualVoxelFace>>();
	VirtualVoxelFaces->Reserve(VoxelPlane);
}

void FVirtualVoxelFaceContainer::AddFace(const int FaceIndex, const FVoxel Voxel, const FIntVector& Position,
	const int Lenght) const
{
	auto& MeshingData = FaceTemplates[FaceIndex].StaticMeshingData;
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, Position, Lenght);
	
	// TODO: remove after unit tests
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);
	
	// Generate new face with coordinates
	if (!VirtualVoxelFaces->IsEmpty())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		FVirtualVoxelFace& PrevFace = VirtualVoxelFaces->Last();

		if (MeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
		{
			// Return when new face was merged
			return;
		}
	}

	VirtualVoxelFaces->Push(NewFace);
}

void FVirtualVoxelFaceContainer::CreateSideFace(const EFaceDirection FaceDirection,
	const FIntVector& VoxelPosition, const FVoxel& Voxel, const int YPosition)
{
	auto& MeshingData = FaceTemplates[FaceDirection].StaticMeshingData;
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, VoxelPosition, YPosition);
	
	// TODO: remove after unit tests are finished
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);

	if (VirtualVoxelFaces->IsEmpty() || !MeshingData.RunDirectionFaceMerge(VirtualVoxelFaces->Last(), NewFace))
	{
		VirtualVoxelFaces->Push(NewFace);
	}
}
