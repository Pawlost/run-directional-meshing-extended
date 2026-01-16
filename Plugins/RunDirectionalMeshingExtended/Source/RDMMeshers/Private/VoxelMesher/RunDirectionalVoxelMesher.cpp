#include "VoxelMesher/RunDirectionalVoxelMesher.h"

FVoxel URunDirectionalVoxelMesher::GetBorderVoxel(FBorderVirtualMeshEventPlanner& IndexParams,
	const FIntVector& BorderVoxelPosition)
{
	const auto VoxelIndex = VoxelData->CalculateVoxelIndex(BorderVoxelPosition);
	return (*VoxelGridPtr)[VoxelIndex];
}

void URunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid)
{
	this->VoxelGridPtr = MakeShared<TArray<FVoxel>>(NewVoxelGrid);
}

void URunDirectionalVoxelMesher::GenerateMesh(FVoxelMeshContainer& MeshContainer, FBorderParams& BorderParameters,
                                              TArray<FRLEVoxelEdit>& VoxelChanges, EBorderVisualizationOption BorderVisualization)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RunDirectionalMeshing generation")
#endif
	{
	#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Edit - RunDirectionalMeshing generation")
	#endif
		
		FScopeLock Lock(&CriticalSection);
		while (!VoxelChanges.IsEmpty())
		{
			const auto& [EditEventIndex, EditVoxel] = VoxelChanges.Pop(EAllowShrinking::No);
			for (uint32 i = 0 ; i < EditVoxel.RunLenght; i++)
			{
				(*VoxelGridPtr)[EditEventIndex + i] = EditVoxel.Voxel;
			}
		}
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from VoxelGrid generation")
#endif

	const auto ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
/*
	for (uint32 z = 0; z < ChunkDimension; z++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			CheckBorderX(VoxelGridObject, MeshVars, y, z);
			CheckBorderY(VoxelGridObject, MeshVars, y, z);
			CheckBorderZ(VoxelGridObject, MeshVars, y, z);
		}
	}

	// Iterate through merged faces
	for (uint8 FaceIndex = 0; FaceIndex < CHUNK_FACE_COUNT; FaceIndex++)
	{
		auto FaceContainer = MeshVars.Faces[FaceIndex];

		// Create quad foreach face
		for (auto Face : *FaceContainer)
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, Face, LocalVoxelTable, FaceIndex);
		}
	}*/
}

/*
void URunDirectionalVoxelMesher::CheckBorderX(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(0, Y, Z);
	const auto ReversePosition = FIntVector(ChunkDimension - 1, Y, Z);
			
	// Back
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, BackFaceTemplate, Position, BorderIndex);

	// Front
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, FrontFaceTemplate, ReversePosition, BorderIndex);
}

void URunDirectionalVoxelMesher::CheckBorderY(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(Y, 0, Z);
	const auto ReversePosition = FIntVector(Y, ChunkDimension - 1, Z);
			
	// Left
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, LeftFaceTemplate, Position, BorderIndex);

	// Right
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, RightFaceTemplate, ReversePosition, BorderIndex);
}

void URunDirectionalVoxelMesher::CheckBorderZ(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(Z, Y, 0);
	const auto ReversePosition = FIntVector(Z, Y, ChunkDimension - 1);
			
	// Bottom
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, BottomFaceTemplate, Position, BorderIndex);

	// Top
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, TopFaceTemplate, ReversePosition, BorderIndex);
}
*/