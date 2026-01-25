#include "VoxelMesher/MeshingUtils//VirtualVoxelFaceContainer.h"
#include "VoxelMesher/MeshingUtils/VoxelMeshContainer.h"

const FStaticMergeData FVirtualVoxelFaceContainer::MeshingDataArray[] = {
	FStaticMergeData::FrontFaceData, FStaticMergeData::BackFaceData,
	FStaticMergeData::RightFaceData, FStaticMergeData::LeftFaceData,
	FStaticMergeData::TopFaceData, FStaticMergeData::BottomFaceData
};

FVirtualVoxelFaceContainer::FVirtualVoxelFaceContainer(uint32 VoxelPlane)
{
	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		VirtualVoxelFaceContainer[f].Reserve(VoxelPlane);	
	}
}

void FVirtualVoxelFaceContainer::AddNewVirtualFace(const EFaceDirection FaceIndex, const FVoxel Voxel, const FIntVector& Position,
                                                   const int Lenght)
{
	auto& MeshingData = MeshingDataArray[FaceIndex];
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, Position, Lenght);

	// TODO: remove after unit tests
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);

	// Generate new face with coordinates
	auto& VirtualVoxelFaces = VirtualVoxelFaceContainer[FaceIndex];
	if (VirtualVoxelFaces.IsEmpty() || !MeshingData.RunDirectionFaceMerge(VirtualVoxelFaces.Last(), NewFace))
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		// Return when new face was merged
		VirtualVoxelFaces.Push(NewFace);
	}
}

void FVirtualVoxelFaceContainer::DirectionalGreedyMergeForVoxelPlane(
	TArray<FVirtualVoxelFace>* ActiveArray, TArray<FVirtualVoxelFace>* PassiveArray,
	FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize, const int MaxVoxelsInChunk)
{
	for (uint8 f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		EFaceDirection FaceDirection = static_cast<EFaceDirection>(f);
		auto& MeshingData = MeshingDataArray[f];

		// Iterate from last face
		auto& VirtualVoxelFaces = VirtualVoxelFaceContainer[f];
		int FaceCount =  VirtualVoxelFaces.Num() - 1;
		
		for (int32 i = FaceCount; i >= 0; i--)
		{
			FVirtualVoxelFace PrevFace = VirtualVoxelFaces.Pop(EAllowShrinking::No);

			while (!ActiveArray->IsEmpty())
			{
				const FVirtualVoxelFace& PopFace = ActiveArray->Pop(EAllowShrinking::No);
				if (MeshingData.MergeFailCondition(PopFace, PrevFace))
				{						
					VoxelMeshContainer.AddVirtualFaceToMesh(PopFace, FaceDirection, VoxelSize, MaxVoxelsInChunk);
				}
				else if (MeshingData.GreedyMerge(PrevFace, PopFace))
				{
					PassiveArray->Append(*ActiveArray);
					ActiveArray->Reset();
				}
				else
				{
					PassiveArray->Push(PopFace);
				}
			}
			
			PassiveArray->Push(PrevFace);
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
}
