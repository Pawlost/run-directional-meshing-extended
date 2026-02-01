#include "VoxelMesher/VirtualChunk/BasicVirtualChunk.h"
#include "VoxelMesher/BasicVoxelMesher.h"

static TArray<TSharedPtr<FBasicVoxelMesher>> UnusedMeshersPool;

FVoxel UBasicVirtualChunk::GetBorderVoxel(FBorderEventMesher& IndexParams,
                                                  const FIntVector& BorderVoxelPosition)
{
	const auto VoxelIndex = VoxelData->CalculateVoxelIndex(BorderVoxelPosition);
	return (*BasicVoxelGridPtr)[VoxelIndex];
}

void UBasicVirtualChunk::CompressVoxelModel(TArray<FVoxel>& NewVoxelGrid)
{
	this->BasicVoxelGridPtr = MakeShared<TArray<FVoxel>>(NewVoxelGrid);
}

void UBasicVirtualChunk::GenerateMesh(FVoxelMesh& MeshContainer, FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelChanges)
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
				(*BasicVoxelGridPtr)[EditEventIndex + i] = EditVoxel.Voxel;
			}
		}
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from VoxelGrid generation")
#endif
	
/*
	for (uint32 z = 0; z < ChunkDimension; z++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			CheckBorderX(VoxelGridObject, MeshVars, y, z);
			CheckBorderY(VoxelGridObject, MeshVars, y, z);
			CheckBorderZ(VoxelGridObject, MeshVars, y, z);
		}
	}*/
	
	
	// This scope may start in a parallel task
	
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif
	
	const uint32 MaxVoxelsInChunk = VoxelData->GetMaxVoxelsInChunk();
	const uint32 VoxelLine = VoxelData->GetVoxelLine();
	const uint32 VoxelPlane = VoxelData->GetVoxelPlane();
	
	TSharedPtr<FBasicVoxelMesher> EventPlanner;
	{
		FScopeLock Lock(&MesherCriticalSection);
		if (!UnusedMeshersPool.IsEmpty())
		{
			EventPlanner = UnusedMeshersPool.Pop();
		}else
		{
			EventPlanner = MakeShared<FBasicVoxelMesher>(VoxelLine, VoxelPlane, MaxVoxelsInChunk);
		}
	}
	
	EventPlanner->VoxelGrid = BasicVoxelGridPtr;
	
	EventPlanner->GenerateVirtualFaces(BorderParameters);
	EventPlanner->ConvertVirtualFacesToMesh(MeshContainer, VoxelData->VoxelSize);
	
	{
		FScopeLock Lock(&MesherCriticalSection);
		constexpr int MAX_NUMBER_OF_MESHERS = 20;
		if (UnusedMeshersPool.Num() < MAX_NUMBER_OF_MESHERS)
		{
			UnusedMeshersPool.Push(EventPlanner);
		}
	}
	/*
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