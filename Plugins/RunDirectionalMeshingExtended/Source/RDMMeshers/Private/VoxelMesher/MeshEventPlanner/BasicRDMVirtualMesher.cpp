#include "VoxelMesher/MeshEventPlanner/BasicRDMVirtualMesher.h"

const TStaticArray<FIntVector, VOXEL_FACE_COUNT> FBasicRDMVirtualMesher::IndexOffset = {
	FIntVector(1, 0, 0), FIntVector(-1, 0, 0), 
	FIntVector(0, 1, 0), FIntVector(0, -1, 0), 
	FIntVector(0, 0, 1), FIntVector(0, 0, -1)
}; 

const TStaticArray<EFaceDirection, VOXEL_FACE_COUNT> FBasicRDMVirtualMesher::InverseDirections = {
	EFaceDirection::Back, EFaceDirection::Front, EFaceDirection::Left, 
	EFaceDirection::Right, EFaceDirection::Bottom, EFaceDirection::Top, 
};

void FBasicRDMVirtualMesher::GenerateVirtualFaces(FBorderParams& BorderParameters)
{
	// Traverse through voxel grid
	
	TStaticArray<FVoxelParams, VOXEL_FACE_COUNT> SideVoxels;
	for (uint32 x = 0; x < VoxelLine; x++)
	{
		for (uint32 z = 0; z < VoxelLine; z++)
		{
			for (uint32 y = 0; y < VoxelLine; y++)
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
			
				VoxelPosition = FIntVector(x, y, z);
				
				UpdateIndexFromPosition();
				
				const auto Voxel = GetCurrentVoxel();
				if (Voxel.IsEmptyVoxel())
				{
					
					// If voxel is empty, no mesh should be generated
						// Calculate indices need to check if face should be generated

						// Check if face should be generated

					InsertNextVoxel(SideVoxels, EFaceDirection::Back, x != 0, FIntVector(0,0,0));
					InsertNextVoxel(SideVoxels, EFaceDirection::Front, x != VoxelLine - 1, FIntVector(1,0,0));
					InsertNextVoxel(SideVoxels, EFaceDirection::Top, z != VoxelLine - 1, FIntVector(0,0,1));
					InsertNextVoxel(SideVoxels, EFaceDirection::Bottom, z != 0, FIntVector(0,0,0));
					InsertNextVoxel(SideVoxels, EFaceDirection::Right, y != VoxelLine - 1, FIntVector(0,1,0));
					InsertNextVoxel(SideVoxels, EFaceDirection::Left, y != 0, FIntVector(0,0,0));
					
					//if(!FullSolid)
					{
						for (uint8 f = 0; f < VOXEL_FACE_COUNT; f++)
						{
							FVoxelParams VoxelParams = SideVoxels[f];
							auto NextVoxel = VoxelParams.NextVoxel;
							// && (Voxel.IsEmptyVoxel() || (Voxel.IsTransparent() && !NextVoxel.IsTransparent()))
							if (NextVoxel != nullptr && !NextVoxel->IsEmptyVoxel()) 
							{
								const auto InverseDirection = InverseDirections[f];
								//  + (OffsetPosition * -2)
								auto OffsetPosition = IndexOffset[InverseDirection];
								VirtualFaces[0].AddNewVirtualFace(InverseDirection, *NextVoxel, VoxelParams.Position, 1);
							}
						}
					}
				}
			}
		}
	}
}

bool FBasicRDMVirtualMesher::InsertNextVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels, const EFaceDirection FaceIndex,
	bool CanGenerate, FIntVector PositionOffset) const
{
	//auto PositionOffset = IndexOffset[FaceIndex];
	const auto AdjustedVoxelIndex = VoxelIndex + CalculateIndexFromPosition(IndexOffset[FaceIndex]);
	
	auto& [Position, NextVoxel] = SideVoxels[FaceIndex];
	Position = FIntVector(0);
	NextVoxel = nullptr;
	
	if (CanGenerate && VoxelGrid->IsValidIndex(AdjustedVoxelIndex))
	{
		// Check if next voxel is visible based on calculated index
		NextVoxel = &(*VoxelGrid)[AdjustedVoxelIndex];
		Position = VoxelPosition + PositionOffset;
		return !NextVoxel->IsEmptyVoxel() && !NextVoxel->IsTransparent();
	}
	
	return false;
}

bool FBasicRDMVirtualMesher::IsVoxelVisible(const int NextVoxelOffset)
{
	return false;
}


void FBasicRDMVirtualMesher::ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize)
{

	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
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
