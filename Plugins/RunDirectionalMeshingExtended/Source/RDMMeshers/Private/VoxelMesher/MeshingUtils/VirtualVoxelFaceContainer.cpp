#include "VoxelMesher/MeshingUtil/VirtualVoxelFaceContext.h"
#include "VoxelMesher/MeshingUtil/FaceMergeData.h"
#include "VoxelMesher/MeshingUtil/VoxelMesh.h"

const FFaceMergeData FVirtualVoxelFaceContext:: FrontFaceData = FFaceMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateFrontFace,
	FVirtualVoxelFace::MergeFailConditionX
);

const FFaceMergeData FVirtualVoxelFaceContext::BackFaceData = FFaceMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBackFace,
	FVirtualVoxelFace::MergeFailConditionX
);

const FFaceMergeData FVirtualVoxelFaceContext::RightFaceData = FFaceMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::CreateRightFace,
	FVirtualVoxelFace::MergeFailConditionRightY
);

const FFaceMergeData FVirtualVoxelFaceContext::LeftFaceData = FFaceMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::CreateLeftFace,
	FVirtualVoxelFace::MergeFailConditionLeftY
);

const FFaceMergeData FVirtualVoxelFaceContext::TopFaceData = FFaceMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateTopFace,
	FVirtualVoxelFace::MergeFailConditionZ
);

const FFaceMergeData FVirtualVoxelFaceContext::BottomFaceData = FFaceMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBottomFace,
	FVirtualVoxelFace::MergeFailConditionZ
);

const FFaceMergeData FVirtualVoxelFaceContext::MeshingDataArray[] = {
	FrontFaceData, BackFaceData,
	RightFaceData, LeftFaceData,
	TopFaceData, BottomFaceData
};

FVirtualVoxelFaceContext::FVirtualVoxelFaceContext(uint32 VoxelPlane)
{
	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		FacesByDirection[f].Reserve(VoxelPlane);	
	}
}

void FVirtualVoxelFaceContext::AddNewVirtualFace(const EFaceDirection FaceIndex, const FVoxel Voxel, const FIntVector& Position,
                                                   const int Lenght)
{
	auto& MeshingData = MeshingDataArray[FaceIndex];
	const FVirtualVoxelFace NewFace = MeshingData.FaceCreator(Voxel, Position, Lenght);

	// TODO: remove after unit tests
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);

	// Generate new face with coordinates
	auto& VirtualVoxelFaces = FacesByDirection[FaceIndex];
	if (VirtualVoxelFaces.IsEmpty() || !MeshingData.RunDirectionFaceMerge(VirtualVoxelFaces.Last(), NewFace))
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		// Return when new face was merged
		VirtualVoxelFaces.Push(NewFace);
	}
}

void FVirtualVoxelFaceContext::DirectionalGreedyMergeForVoxelPlane(
	TArray<FVirtualVoxelFace>* ActiveArray, TArray<FVirtualVoxelFace>* PassiveArray,
	FVoxelMesh& VoxelMeshContainer, const double VoxelSize, const int MaxVoxelsInChunk)
{
	for (uint8 f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		EFaceDirection FaceDirection = static_cast<EFaceDirection>(f);
		auto& MeshingData = MeshingDataArray[f];

		// Iterate from last face
		auto& VirtualVoxelFaces = FacesByDirection[f];
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
