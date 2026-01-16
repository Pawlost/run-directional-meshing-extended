#include "VoxelMesher/MeshEventPlanner/BasicRDMVirtualMesher.h"

void FBasicRDMVirtualMesher::GenerateVirtualFaces(FBorderParams& BorderParameters)
{
	// Traverse through voxel grid
	int SignedVoxelLine = VoxelLine;
	int SignedVoxelPlane = VoxelPlane;
	for (VoxelPosition.X = 0; VoxelPosition.X < SignedVoxelLine; VoxelPosition.X++)
	{
		for (VoxelPosition.Z = 0; VoxelPosition.Z < SignedVoxelLine; VoxelPosition.Z++)
		{
			for (VoxelPosition.Y = 0; VoxelPosition.Y < SignedVoxelLine; VoxelPosition.Y++)
			{
				/*
				* Increment run for each chunk axis
				* Coordinates are different in order to create a sorted array of quads/faces.
				* Sorted array is then used in Run Direction Greedy meshing, because it is sorted the array needs to traversed only once.
				* Grid is only read from, it doesn't matter which coordinate is first
				* Each voxel needs to be visited only once per face.
				* Indices are same for face and reversed face.
				*
				* The run direction is from left to right, bottom to top and left to right.
				*/
			
				UpdateIndexFromPosition();
				IncrementRun(EFaceDirection::Left, VoxelPosition.Y, -1);
				IncrementRun(EFaceDirection::Right, VoxelPosition.Y, 1);
				IncrementRun(EFaceDirection::Front, VoxelPosition.X, SignedVoxelPlane);
				IncrementRun(EFaceDirection::Back, VoxelPosition.X, -SignedVoxelPlane);
				IncrementRun(EFaceDirection::Top, VoxelPosition.Z, SignedVoxelLine);
				IncrementRun(EFaceDirection::Bottom, VoxelPosition.Z, -SignedVoxelLine);
			}
		}
	}
}

bool FBasicRDMVirtualMesher::IsVoxelVisible(const int NextVoxelOffset)
{
	int NextVoxelIndex = CurrentIndex + NextVoxelOffset;
	if (VoxelGrid->IsValidIndex(NextVoxelIndex))
	{
		// Check if next voxel is visible based on calculated index
		const auto NextVoxel = VoxelGrid->GetData()[NextVoxelIndex];
		return NextVoxel.IsTransparent() && NextVoxel != GetCurrentVoxel();
	}
	return false;
}


void FBasicRDMVirtualMesher::ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize)
{

	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < VoxelLine; y++)
		{
			VirtualFaces[y].DirectionalGreedyMergeForVoxelPlane(FirstDirectionalMeshingHelperArray,
																   SecondDirectionalMeshingHelperArray,
																   VoxelMeshContainer, static_cast<EFaceDirection>(f),
																   VoxelSize, MaxVoxelsInChunk);
		}
	}
}

void FBasicRDMVirtualMesher::IncrementRun(const EFaceDirection& FaceIndex, int VoxelPlaneIndex, int NextVoxelOffset)
{
	// If voxel is empty, no mesh should be generated
	const auto Voxel = GetCurrentVoxel();
	if (!Voxel.IsEmptyVoxel())
	{
		// Calculate indices need to check if face should be generated

		// Check if face should be generated
		if (IsVoxelVisible(NextVoxelOffset))
		{
			VirtualFaces[VoxelPlaneIndex].AddNewVirtualFace(FaceIndex, Voxel, VoxelPosition, 1);
		}
	}
}

/*
void FBasicRDMVirtualMesher::IncrementBorderRun(const EFaceDirection& FaceTemplate,
                                                const FIntVector& Position, const int BorderIndex) const
{
	const int32 Index = VoxelGenerator->CalculateVoxelIndex(Position);
	const FVoxel Voxel = (*VoxelGrid)[Index];
			
	// Check voxel visibility in side chunk (crosschunk)
	const auto FaceContainerIndex = static_cast<uint8>(FaceTemplate.StaticMeshingData.FaceDirection);

	const FVoxelParams VoxelParams =
	{
		Voxel,
		Position
	};
			
	const FBorderVoxelIndexParams BorderVoxelIndexParams =
	{
		BorderIndex,
		FaceTemplate.StaticMeshingData,
		VoxelParams
	};
			
	if (IsBorderVoxelVisible(BorderVoxelIndexParams, MeshVars.ChunkParams))
	{
		AddFace(FaceTemplate, VoxelParams, MeshVars.Faces[FaceContainerIndex]);
	}
}
*/

/*
bool URunDirectionalVoxelMesher::IsBorderVoxelVisible(const FBorderVoxelIndexParams& FaceData,
													  const FChunkParams& ChunkStruct)
{
	// Check voxel visibility in side chunk (crosschunk)
	const auto FaceContainerIndex = static_cast<uint8>(FaceData.StaticData.FaceDirection);
	const auto SideChunk = ChunkStruct.SideChunks[FaceContainerIndex];
	if (SideChunk != nullptr && SideChunk->VoxelModel != nullptr)
	{
		const auto& NextVoxel = SideChunk->VoxelModel->GetVoxelAtIndex(FaceData.SideChunkVoxelIndex);
		return NextVoxel.IsTransparent() && NextVoxel != FaceData.VoxelParams.CurrentVoxel;
	}

	return SideChunk == nullptr && ChunkStruct.ShowBorders;
}
*/
