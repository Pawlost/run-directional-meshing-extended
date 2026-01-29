#include "VoxelMesher/MeshEventPlanner/BasicRDMVirtualMesher.h"

const TStaticArray<FBasicRDMVirtualMesher::FVoxelSideParams, VOXEL_FACE_COUNT>
FBasicRDMVirtualMesher::VoxelSideParams = {
	FVoxelSideParams{
		EFaceDirection::Front, EFaceDirection::Back, FIntVector(1, 0, 0), FVirtualVoxelFace::GetXFromVector
	},
	FVoxelSideParams{
		EFaceDirection::Back, EFaceDirection::Front, FIntVector(-1, 0, 0), FVirtualVoxelFace::GetXFromVector
	},
	FVoxelSideParams{
		EFaceDirection::Right, EFaceDirection::Left, FIntVector(0, 1, 0), FVirtualVoxelFace::GetYFromVector
	},
	FVoxelSideParams{
		EFaceDirection::Left, EFaceDirection::Right, FIntVector(0, -1, 0), FVirtualVoxelFace::GetYFromVector
	},
	FVoxelSideParams{
		EFaceDirection::Top, EFaceDirection::Bottom, FIntVector(0, 0, 1), FVirtualVoxelFace::GetZFromVector
	},
	FVoxelSideParams{
		EFaceDirection::Bottom, EFaceDirection::Top, FIntVector(0, 0, -1), FVirtualVoxelFace::GetZFromVector
	}
};

void FBasicRDMVirtualMesher::CheckBorder(FBorderParams& BorderParameters, EFaceDirection Direction,
                                         const FIntVector& CurrentPosition,
                                         const FIntVector& BorderVoxelPosition)
{
	auto CurrentVoxel = GetVoxelFromPosition(CurrentPosition);
	if (!CurrentVoxel.IsEmptyVoxel())
	{
		if (BorderParameters.CanGenerateBorder(Direction, BorderVoxelPosition, CurrentVoxel))
		{
			VirtualFaces[0].AddNewVirtualFace(Direction, CurrentVoxel,
			                                  CurrentPosition + VoxelPositionOffsets[Direction], 1);
		}
	}
}

void FBasicRDMVirtualMesher::GenerateVirtualFaces(FBorderParams& BorderParameters)
{
	// Traverse through voxel grid
	TStaticArray<FVoxelParams, VOXEL_FACE_COUNT> TempVoxels;
	for (uint32 x = 0; x < VoxelLine; x++)
	{
		for (uint32 z = 0; z < VoxelLine; z++)
		{
			// Front border
			CheckBorder(BorderParameters, EFaceDirection::Back, FIntVector(0, z, x), FIntVector(VoxelLine - 1, z, x));
			CheckBorder(BorderParameters, EFaceDirection::Front, FIntVector(VoxelLine - 1, z, x), FIntVector(0, z, x));
			CheckBorder(BorderParameters, EFaceDirection::Left, FIntVector(x, 0, z), FIntVector(x, VoxelLine - 1, z));
			CheckBorder(BorderParameters, EFaceDirection::Right, FIntVector(x, VoxelLine - 1, z), FIntVector(x, 0, z));
			CheckBorder(BorderParameters, EFaceDirection::Bottom, FIntVector(x, z, 0), FIntVector(x, z, VoxelLine - 1));
			CheckBorder(BorderParameters, EFaceDirection::Top, FIntVector(x, z, VoxelLine - 1), FIntVector(x, z, 0));

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

				// If voxel is empty, no mesh should be generated
				// Calculate indices need to check if face should be generated

				// Check if face should be generated

				if (Voxel.IsEmptyVoxel() || Voxel.IsTransparent())
				{
					// TODO: finish
					bool FullSolid = false;
					CheckInnerVoxel(TempVoxels, EFaceDirection::Back, x != 0);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Front, x != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Top, z != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Bottom, z != 0);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Right, y != VoxelLine - 1);
					CheckInnerVoxel(TempVoxels, EFaceDirection::Left, y != 0);

					if (!FullSolid)
					{
						for (uint8 f = 0; f < VOXEL_FACE_COUNT; f++)
						{
							FVoxelParams VoxelParams = TempVoxels[f];
							auto NextVoxel = VoxelParams.NextVoxel;
							if (VoxelParams.IsValid && !NextVoxel.IsEmptyVoxel() && (Voxel.IsEmptyVoxel() || (Voxel.
								IsTransparent() && !NextVoxel.IsTransparent())))
							{
								const FVoxelSideParams& VoxelSideParam = VoxelSideParams[f];
								const auto InverseDirection = VoxelSideParam.InverseDirection;
								int VoxelPlaneIndex = VoxelSideParam.GetVoxelPlaneFromPosition(VoxelPosition);
								VirtualFaces[VoxelPlaneIndex].AddNewVirtualFace(
									InverseDirection, NextVoxel, VoxelPosition + VoxelPositionOffsets[f], 1);
							}
						}
					}
				}
			}
		}
	}
}

bool FBasicRDMVirtualMesher::CheckInnerVoxel(TStaticArray<FVoxelParams, VOXEL_FACE_COUNT>& SideVoxels,
                                             const EFaceDirection FaceIndex,
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
