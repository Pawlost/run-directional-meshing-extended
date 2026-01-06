#include "VoxelMesher/VoxelMesherBase.h"
#include "BaseVoxelData.h"

void UVoxelMesherBase::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
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

void UVoxelMesherBase::UpdateFaceParams(FMeshingDirections& Face, const FIntVector ForwardVoxelIndexVector,
                                        const FIntVector ChunkBorderIndexVector,
                                        const FIntVector PreviousVoxelIndexVector) const
{
	Face.ForwardVoxelIndex = VoxelData->CalculateVoxelIndex(ForwardVoxelIndexVector);
	Face.PreviousVoxelIndex = VoxelData->CalculateVoxelIndex(PreviousVoxelIndexVector);
	Face.ChunkBorderIndex = VoxelData->CalculateVoxelIndex(ChunkBorderIndexVector);
}

void UVoxelMesherBase::PreallocateArrays(
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
	TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation preallocation")
#endif

	auto ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	auto ChunkLayer = VoxelData->GetVoxelCountPerVoxelPlane();
	
	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		auto FaceArray = VirtualFaces[f];

		SideFaces[f] = MakeShared<TArray<FVirtualVoxelFace>>();
		SideFaces[f]->Reserve(ChunkLayer);

		if (FaceArray == nullptr || !FaceArray.IsValid())
		{
			VirtualFaces[f] = MakeShared<TArray<TArray<FVirtualVoxelFace>>>();
		}
		else
		{
			// If array was pulled from a pool, just empty it 
			VirtualFaces[f]->Reset();
		}

		VirtualFaces[f]->SetNum(ChunkDimension);
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			(*VirtualFaces[f])[y].Reserve(ChunkLayer);
		}
	}
}