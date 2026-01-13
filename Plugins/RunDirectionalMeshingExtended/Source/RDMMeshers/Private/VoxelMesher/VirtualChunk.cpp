#include "VoxelMesher/VirtualChunk.h"
#include "BaseVoxelData.h"

void UVirtualChunk::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
{
	this->VoxelData = VoxelGeneratorBase;
	//UpdateAllFacesParams();
}

/*
void UVoxelMesherBase::UpdateAllFacesParams()
{
	const auto ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	//Axis X
	UpdateFaceParams(FaceTemplates[EFaceDirection::Front], FIntVector(1, 0, 0),
	                 FIntVector(0, 0, 0),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Back], FIntVector(-1, 0, 0),
	                 FIntVector(ChunkDimension - 1, 0, 0),
	                 FIntVector(0, -1, 0));

	//Axis Y
	UpdateFaceParams(FaceTemplates[EFaceDirection::Right], FIntVector(0, 1, 0),
	                 FIntVector(0, 0, 0),
	                 FIntVector(-1, 0, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Left], FIntVector(0, -1, 0),
	                 FIntVector(0, ChunkDimension - 1, 0),
	                 FIntVector(-1, 0, 0));

	// Axis Z
	UpdateFaceParams(FaceTemplates[EFaceDirection::Bottom], FIntVector(0, 0, -1),
	                 FIntVector(0, 0, ChunkDimension - 1),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Top], FIntVector(0, 0, 1),
	                 FIntVector(0, 0, 0),
	                 FIntVector(0, -1, 0));
}*/

void UVirtualChunk::UpdateFaceParams(FMeshingDirections& Face, const FIntVector ForwardVoxelIndexVector,
                                        const FIntVector ChunkBorderIndexVector,
                                        const FIntVector PreviousVoxelIndexVector) const
{
	Face.ForwardVoxelIndex = VoxelData->CalculateVoxelIndex(ForwardVoxelIndexVector);
	Face.PreviousVoxelIndex = VoxelData->CalculateVoxelIndex(PreviousVoxelIndexVector);
	Face.ChunkBorderIndex = VoxelData->CalculateVoxelIndex(ChunkBorderIndexVector);
}