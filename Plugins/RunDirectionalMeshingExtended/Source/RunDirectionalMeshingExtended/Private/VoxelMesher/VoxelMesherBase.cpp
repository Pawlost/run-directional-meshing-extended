#include "VoxelMesher/VoxelMesherBase.h"

#include "Log/VoxelMeshingProfilingLogger.h"
#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Spawner/ChunkSpawnerBase.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/VoxelGrid.h"

void UVoxelMesherBase::SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase)
{
	this->VoxelGenerator = VoxelGeneratorBase;
	UpdateAllFacesParams();
}

void UVoxelMesherBase::CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid)
{

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Voxel compression generation")
#endif

	// Unoptimized because it is out of scope for this thesis
	auto VoxelGridObject = NewObject<UVoxelGrid>();
	VoxelGridObject->VoxelGrid = MakeShared<TArray<FVoxel>>();
	VoxelGridObject->VoxelGrid->Append(VoxelGrid);
	Chunk.VoxelModel = VoxelGridObject;

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	const FString MapName = GetWorld()->GetMapName();
	FVoxelMeshingProfilingLogger::LogAllocatedMemory(MapName, VoxelGridObject->VoxelGrid->GetAllocatedSize());

	const uint32 VoxelCount = VoxelGenerator->GetVoxelCountPerChunk();
	uint32 OpaqueVoxelCount = 0;
	for (const auto OpaqueVoxels : Chunk.ChunkVoxelIdTable)
	{
		OpaqueVoxelCount += OpaqueVoxels.Value;
	}
	FVoxelMeshingProfilingLogger::LogVoxelSparsity(MapName, OpaqueVoxelCount, VoxelCount - OpaqueVoxelCount);
#endif
	
}

const UVoxelMesherBase::FNormalsAndTangents UVoxelMesherBase::FaceNormalsAndTangents[] = {
	{FVector(1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 1.0, 0.0)}, //Front
	{FVector(-1.0f, 0.0f, 0.0f), FProcMeshTangent(0.0, 1.0, 0.0)}, //Back
	{FVector(0.0f, 1.0f, 0.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)}, // Right 
	{FVector(0.0f, -1.0f, 0.0f), FProcMeshTangent(1.0, 0.0, 0.0)}, // Left
	{FVector(0.0f, 0.0f, -1.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)}, //Bottom
	{FVector(0.0f, 0.0f, 1.0f), FProcMeshTangent(1.0f, 0.0f, 0.0f)} //Top
};

void UVoxelMesherBase::UpdateAllFacesParams()
{
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	//Axis X
	UpdateFaceParams(FrontFaceTemplate, FIntVector(1, 0, 0),
					 FIntVector(0, 0, 0),
					FIntVector(0, -1, 0));

	UpdateFaceParams(BackFaceTemplate, FIntVector(-1, 0, 0),
				FIntVector(ChunkDimension - 1, 0, 0),
				FIntVector(0, -1, 0));

	//Axis Y
	UpdateFaceParams(RightFaceTemplate, FIntVector(0, 1, 0),
					 FIntVector(0, 0, 0),
					 FIntVector(-1, 0, 0));

	UpdateFaceParams(LeftFaceTemplate,FIntVector(0, -1, 0),
					 FIntVector(0, ChunkDimension - 1, 0),
					 FIntVector(-1, 0, 0));

	// Axis Z
	UpdateFaceParams(BottomFaceTemplate, FIntVector(0, 0, -1),
	                 FIntVector(0, 0, ChunkDimension - 1),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(TopFaceTemplate, FIntVector(0, 0, 1),
	                 FIntVector(0, 0, 0),
	                 FIntVector(0, -1, 0));
}

void UVoxelMesherBase::UpdateFaceParams(FMeshingDirections& Face, const FIntVector ForwardVoxelIndexVector,
                                   const FIntVector ChunkBorderIndexVector,
                                   const FIntVector PreviousVoxelIndexVector) const
{
	Face.ForwardVoxelIndex = VoxelGenerator->CalculateVoxelIndex(ForwardVoxelIndexVector);
	Face.PreviousVoxelIndex = VoxelGenerator->CalculateVoxelIndex(PreviousVoxelIndexVector);
	Face.ChunkBorderIndex = VoxelGenerator->CalculateVoxelIndex(ChunkBorderIndexVector);
}

bool UVoxelMesherBase::EmptyActor(const FMesherVariables& MeshVars)
{
	MeshVars.ChunkParams.OriginalChunk->bHasMesh = false;

	if (MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.IsEmpty())
	{
		if (MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor.IsValid())
		{
			// If chunk is full of empty voxels but actor was pulled from pool, clear its mesh
			MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor->ClearMesh();
		}
		return true;
	}

	return false;
}

void UVoxelMesherBase::InitFaceContainers(FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation preallocation")
#endif

	MeshVars.VoxelIdToLocalVoxelMap.Reserve(MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.Num());
	MeshVars.VoxelIdToLocalVoxelMap.Empty();

	for (const auto VoxelId : MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable)
	{
		const auto LocalVoxelId = MeshVars.VoxelIdToLocalVoxelMap.Num();
		MeshVars.VoxelIdToLocalVoxelMap.Add(VoxelId.Key, LocalVoxelId);
	}

	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		TMap<int32, uint32>& VoxelTable = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable;

		auto FaceArray = MeshVars.Faces[f];
		if (FaceArray == nullptr || !FaceArray.IsValid())
		{
			// In case voxel table is not available this code needs to be rewritten to add local voxels id dynamically during voxel grid traversal
			FaceArray = MakeShared<TArray<FVoxelFace>>();
			MeshVars.Faces[f] = FaceArray;
		}
		else
		{
			// If array was pulled from a pool, just empty it 
			FaceArray->Empty();
		}

		// Preallocate memory needed for meshing
		const uint32 EstimatedVoxels = VoxelGenerator->GetVoxelCountPerChunk() * VoxelTable.Num();
		FaceArray->Reserve(EstimatedVoxels);
	}
}

void UVoxelMesherBase::GenerateActorMesh(const TSharedPtr<FChunkParams>& ChunkParams) const
{
	const auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	//Spawn actor
	const auto Chunk = ChunkParams->OriginalChunk;
	TWeakObjectPtr<AChunkRMCActor> ActorPtr = Chunk->ChunkMeshActor;
	const auto SpawnLocation = FVector(Chunk->GridPosition) * VoxelGenerator->GetChunkAxisSize();

	FAttachmentTransformRules ActorAttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	if (!ChunkParams->WorldTransform)
	{
		ActorAttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	}

	if (ActorPtr == nullptr)
	{
		// If there is no actor spawn new one.
		ActorPtr = World->SpawnActor<AChunkRMCActor>(AChunkRMCActor::StaticClass(), SpawnLocation,
		                                             FRotator::ZeroRotator);

		if (!ActorPtr.IsValid() || !ChunkParams->SpawnerPtr.IsValid())
		{
			return;
		}

		ActorPtr->AttachToActor(ChunkParams->SpawnerPtr.Get(), ActorAttachmentRules);
	}
	else
	{
		if (!ActorPtr.IsValid())
		{
			return;
		}
		
		// If actor exists, ensure correct location
		if (!ChunkParams->WorldTransform)
		{
			ActorPtr->SetActorRelativeLocation(SpawnLocation);
		}
		else
		{
			ActorPtr->SetActorLocation(SpawnLocation);
		}
	}
	
	Chunk->ChunkMeshActor = ActorPtr;


	ActorPtr->PrepareMesh();
}
