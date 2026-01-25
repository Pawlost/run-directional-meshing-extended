#include "VoxelMesher/MeshEventPlanner/BasicRDMVirtualMesher.h"


const TStaticArray<FBasicRDMVirtualMesher::FVoxelSideParams, VOXEL_FACE_COUNT> 
FBasicRDMVirtualMesher::VoxelSideParams = {
	FVoxelSideParams{EFaceDirection::Front, EFaceDirection::Back, FIntVector(1, 0, 0)  ,  FIntVector(1,0,0)  ,   FVirtualVoxelFace::GetXFromVector   },
	FVoxelSideParams{EFaceDirection::Back, EFaceDirection::Front, FIntVector(-1, 0, 0),    FIntVector(0,0,0) ,   FVirtualVoxelFace::GetXFromVector  },
	FVoxelSideParams{EFaceDirection::Right, EFaceDirection::Left, FIntVector(0, 1, 0)  ,  FIntVector(0,1,0)  ,   FVirtualVoxelFace::GetYFromVector  },
	FVoxelSideParams{EFaceDirection::Left, EFaceDirection::Right, FIntVector(0, -1, 0),   FIntVector(0,0,0)  ,  FVirtualVoxelFace::GetYFromVector  },
	FVoxelSideParams{EFaceDirection::Top, EFaceDirection::Bottom, FIntVector(0, 0, 1),     FIntVector(0,0,1) ,  FVirtualVoxelFace::GetZFromVector  },
	FVoxelSideParams{EFaceDirection::Bottom, EFaceDirection::Top, FIntVector(0, 0, -1)  , FIntVector(0,0,0)  ,    FVirtualVoxelFace::GetZFromVector}
};

void FBasicRDMVirtualMesher::GenerateVirtualFaces(FBorderParams& BorderParameters)
{
	// Traverse through voxel grid
	TStaticArray<FVoxelParams, VOXEL_FACE_COUNT> TempVoxels;
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
					FIntVector SidePosition = VoxelPosition;
					CheckBorderVoxel(TempVoxels, EFaceDirection::Back, x == 0, BorderParameters, SidePosition);
					CheckBorderVoxel(TempVoxels, EFaceDirection::Front, x == VoxelLine - 1, BorderParameters, SidePosition);
					CheckBorderVoxel(TempVoxels, EFaceDirection::Top, z == VoxelLine - 1, BorderParameters, SidePosition);
					CheckBorderVoxel(TempVoxels, EFaceDirection::Bottom, z == 0, BorderParameters, SidePosition);
					CheckBorderVoxel(TempVoxels, EFaceDirection::Right, y == VoxelLine - 1, BorderParameters, SidePosition);
					CheckBorderVoxel(TempVoxels, EFaceDirection::Left, y == 0, BorderParameters, SidePosition);
					
					CheckInnerVoxel(TempVoxels, EFaceDirection::Back, x != 0);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Front, x != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Top, z != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Bottom, z != 0);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Right, y != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Left, y != 0);
					
					//if(!FullSolid)
					{
						for (uint8 f = 0; f < VOXEL_FACE_COUNT; f++)
						{
							FVoxelParams VoxelParams = TempVoxels[f];
							auto NextVoxel = VoxelParams.NextVoxel;
							// && (Voxel.IsEmptyVoxel() || (Voxel.IsTransparent() && !NextVoxel.IsTransparent()))
							if (VoxelParams.IsValid && !NextVoxel.IsEmptyVoxel()) 
							{
								const FVoxelSideParams& VoxelSideParam = VoxelSideParams[f];
								const auto InverseDirection = VoxelSideParam.InverseDirection;
								int VoxelPlaneIndex = VoxelParams.IsBorderVoxel ? 0 : VoxelSideParam.GetVoxelPlaneFromPosition(VoxelPosition);
								VirtualFaces[VoxelPlaneIndex].AddNewVirtualFace(InverseDirection, NextVoxel, VoxelPosition + VoxelSideParam.PositionOffset, 1);
							}
						}
					}
				}
			}
		}
	}
}

bool FBasicRDMVirtualMesher::CheckInnerVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels, const EFaceDirection FaceIndex,
	bool CanCheck) const
{
	auto& [NextVoxel, IsBorderVoxel, IsValid] = SideVoxels[FaceIndex];
	IsValid = false;
	
	if (CanCheck)
	{
		const FVoxelSideParams& VoxelSideParam = VoxelSideParams[FaceIndex];
		const auto AdjustedVoxelIndex = VoxelIndex + CalculateIndexFromPosition(VoxelSideParam.IndexOffset);
		
		if (VoxelGrid->IsValidIndex(AdjustedVoxelIndex))
		{
			IsBorderVoxel = false;
			IsValid = true;
			// Check if next voxel is visible based on calculated index
			NextVoxel = (*VoxelGrid)[AdjustedVoxelIndex];
			return !NextVoxel.IsEmptyVoxel() && !NextVoxel.IsTransparent();
		}
	}
	
	return !CanCheck;
}

bool FBasicRDMVirtualMesher::CheckBorderVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels, const EFaceDirection FaceIndex,
	bool CanCheckBorder, FBorderParams& BorderParameters, FIntVector SideChunkBorderPosition) const
{
	/*
	if (CanCheckBorder && BorderParameters.IsBorderValid(FaceIndex))
	{
		const FVoxelSideParams& VoxelSideParam= VoxelSideParams[FaceIndex];
		auto& [Position, NextVoxel, IsBorderVoxel, IsValid] = SideVoxels[FaceIndex];
		IsValid = true;
		IsBorderVoxel = true;
		
		// Check if next voxel is visible based on calculated index
		NextVoxel = BorderParameters.GetBorderVoxel(FaceIndex, SideChunkBorderPosition); 
		Position = SideChunkBorderPosition;
		return !NextVoxel.IsEmptyVoxel() && !NextVoxel.IsTransparent();
	}*/
	
	return !CanCheckBorder;
}

void FBasicRDMVirtualMesher::ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer, const double VoxelSize)
{
	for (uint32 y = 0; y < VoxelLine; y++)
	{
		VirtualFaces[y].DirectionalGreedyMergeForVoxelPlane(&FirstDirectionalMeshingHelperArray,
															   &SecondDirectionalMeshingHelperArray,
															   VoxelMeshContainer,
															   VoxelSize, MaxVoxelsInChunk);
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
