#include "VoxelMesher/MeshingUtils//VirtualVoxelFaceContainer.h"

#include "VoxelMesher/MeshingUtils/VoxelMeshContainer.h"

FVirtualVoxelFaceContainer::FVirtualVoxelFaceContainer(uint32 VoxelPlane)
{
	VirtualVoxelFaces.Reserve(VoxelPlane);
}

void FVirtualVoxelFaceContainer::AddFace(const int FaceIndex, const FVoxel Voxel, const FIntVector& Position,
	const int Lenght)
{
	auto& MeshingData = MeshingDataArray[FaceIndex];
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, Position, Lenght);
	
	// TODO: remove after unit tests
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);
	
	// Generate new face with coordinates
	if (!VirtualVoxelFaces.IsEmpty())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		FVirtualVoxelFace& PrevFace = VirtualVoxelFaces.Last();

		if (MeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
		{
			// Return when new face was merged
			return;
		}
	}

	VirtualVoxelFaces.Push(NewFace);
}

void FVirtualVoxelFaceContainer::CreateSideFace(const EFaceDirection FaceDirection,
	const FIntVector& VoxelPosition, const FVoxel& Voxel, const int YPosition)
{
	auto& MeshingData = MeshingDataArray[FaceDirection];
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, VoxelPosition, YPosition);
	
	// TODO: remove after unit tests are finished
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);

	if (VirtualVoxelFaces.IsEmpty() || !MeshingData.RunDirectionFaceMerge(VirtualVoxelFaces.Last(), NewFace))
	{
		VirtualVoxelFaces.Push(NewFace);
	}
}

void FVirtualVoxelFaceContainer::DirectionalGreedyMergeForVoxelPlane(
	TArray<FVirtualVoxelFace>& FirstArray, TArray<FVirtualVoxelFace>& SecondArray, FVoxelMeshContainer& VoxelMeshContainer,
	 const EFaceDirection FaceDirection, const double VoxelSize, const int MaxVoxelsInChunk)
{
	TArray<FVirtualVoxelFace>* ActiveArray = &FirstArray;
	TArray<FVirtualVoxelFace>* PassiveArray = &SecondArray;
	
	auto& MeshingData = MeshingDataArray[FaceDirection];

	// Iterate from last face
	for (int32 i = VirtualVoxelFaces.Num() - 1; i >= 0; i--)
	{
		FVirtualVoxelFace PrevFace = VirtualVoxelFaces.Pop(EAllowShrinking::No);

		if (ActiveArray->IsEmpty() || MeshingData.HeightCondition(ActiveArray->Top(), PrevFace))
		{
			ActiveArray->Push(PrevFace);
		}
		else
		{
			while (!ActiveArray->IsEmpty())
			{
				const FVirtualVoxelFace& PopFace = ActiveArray->Pop(EAllowShrinking::No);
				if (!MeshingData.MergeFailCondition(PopFace, PrevFace))
				{
					// Attempt greedy merge
					if (MeshingData.GreedyMerge(PrevFace, PopFace))
					{
						PassiveArray->Append(*ActiveArray);
						ActiveArray->Reset();
					}
					else
					{
						PassiveArray->Push(PopFace);
					}
				}
				else
				{
					VoxelMeshContainer.AddVirtualFaceToMesh(PopFace, FaceDirection, VoxelSize, MaxVoxelsInChunk);
				}
			}
			
			PassiveArray->Push(PrevFace);
		}

		Swap(PassiveArray, ActiveArray);
	}

	while (!ActiveArray->IsEmpty())
	{
		const FVirtualVoxelFace& PopFace = ActiveArray->Pop(EAllowShrinking::No);
		VoxelMeshContainer.AddVirtualFaceToMesh(PopFace, FaceDirection, VoxelSize, MaxVoxelsInChunk);
	}

	while (!PassiveArray->IsEmpty())
	{
		const FVirtualVoxelFace& PopFace = PassiveArray->Pop(EAllowShrinking::No);
		VoxelMeshContainer.AddVirtualFaceToMesh(PopFace, FaceDirection, VoxelSize, MaxVoxelsInChunk);
	}
}
