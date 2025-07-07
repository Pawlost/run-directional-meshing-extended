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
		// Voxel faces need to be sorted to different arrays by Id because Realtime Mesh Component requires it
		for (const auto Voxel : MeshVars.VoxelIdToLocalVoxelMap)
		{
			TMap<int32, uint32>& VoxelTable = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable;
			MeshVars.Faces[f].SetNum(VoxelTable.Num());

			auto FaceArray = MeshVars.Faces[f][Voxel.Value];
			if (FaceArray == nullptr || !FaceArray.IsValid())
			{
				// In case voxel table is not available this code needs to be rewritten to add local voxels id dynamically during voxel grid traversal
				FaceArray = MakeShared<TArray<FVoxelFace>>();
				MeshVars.Faces[f][Voxel.Value] = FaceArray;
			}
			else
			{
				// If array was pulled from a pool, just empty it 
				FaceArray->Empty();
			}

			// Preallocate memory needed for meshing
			const uint32 Count = VoxelGenerator->GetVoxelCountPerChunk();
			FaceArray->Reserve(Count);
		}
	}
}

void UVoxelMesherBase::GenerateMeshFromFaces(const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Buffer - UE RunDirectionalMeshing generation")
#endif
	
	if (!IsValid(VoxelGenerator))
	{
		return;
	}

	auto VoxelSize = VoxelGenerator->GetVoxelSize();

	auto Spawner = MakeShared<FChunkParams>(MeshVars.ChunkParams);

	if (!MeshVars.ChunkParams.ExecutedOnMainThread)
	{

		// Synchronize Mesh generation with game thread.
		Async(EAsyncExecution::TaskGraphMainThread, [this, Spawner]()
		{
			GenerateActorMesh(Spawner);
		}).Wait();
	}
	else
	{
		//Creating AsyncTask from main thread will cause deadlock
		GenerateActorMesh(Spawner);
	}
	
	// Local voxel table 
	TMap<uint32, uint16> LocalVoxelTable;
	SIZE_T GlobalVertexCount = 0;

	auto MeshActor = MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor;
	
	// Iterate through merged faces
	for (auto VoxelId : MeshVars.VoxelIdToLocalVoxelMap)
	{
		TSharedPtr<TArray<FVector>> Vertices = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<int32>> Triangles = MakeShared<TArray<int32>>();
		TSharedPtr<TArray<FVector2D>> UV0 = MakeShared<TArray<FVector2D>>();
		TSharedPtr<TArray<FVector>> Normals = MakeShared<TArray<FVector>>();
		TSharedPtr<TArray<FProcMeshTangent>> Tangents = MakeShared<TArray<FProcMeshTangent>>();

		constexpr int VERTICES_PER_VOXEL = 24;
		const int VERTICES_PER_CHUNK = VoxelGenerator->GetVoxelCountPerChunk()*VERTICES_PER_VOXEL; 
		Vertices->Reserve(VERTICES_PER_CHUNK);
		Triangles->Reserve(VERTICES_PER_CHUNK);
		UV0->Reserve(VERTICES_PER_CHUNK);
		Normals->Reserve(VERTICES_PER_CHUNK);
		Tangents->Reserve(VERTICES_PER_CHUNK);
		
		int64 TriangleIndex = 0;
		
		for (uint8 FaceIndex = 0; FaceIndex < CHUNK_FACE_COUNT; FaceIndex++)
		{
			auto FaceContainer = MeshVars.Faces[FaceIndex];

			auto SideFaces = FaceContainer[VoxelId.Value];

			auto [Normal, Tangent] = FaceNormalsAndTangents[FaceIndex];

			// Create quad foreach face
			for (auto Face : *SideFaces)
			{
				// Create quad from 2 triangles
				
				Vertices->Add(Face.GetFinalStartVertexDown(VoxelSize));
				Vertices->Add(Face.GetFinalEndVertexDown(VoxelSize));
				Vertices->Add(Face.GetFinalEndVertexUp(VoxelSize));
				Vertices->Add(Face.GetFinalStartVertexUp(VoxelSize));

				Triangles->Add(TriangleIndex);
				Triangles->Add(TriangleIndex + 1);
				Triangles->Add(TriangleIndex + 2);
				Triangles->Add(TriangleIndex + 2);
				Triangles->Add(TriangleIndex + 3);
				Triangles->Add(TriangleIndex);

				Normals->Add(Normal);
				Normals->Add(Normal);
				Normals->Add(Normal);
				Normals->Add(Normal);

				Tangents->Add(Tangent);
				Tangents->Add(Tangent);
				Tangents->Add(Tangent);
				Tangents->Add(Tangent);
				
				UV0->Add(FVector2D(0, 0));
				UV0->Add(FVector2D(1, 0));
				UV0->Add(FVector2D(1, 1));
				UV0->Add(FVector2D(0, 1));
				
				TriangleIndex+=4;
				
				if (!LocalVoxelTable.Contains(VoxelId.Key))
				{
					// Keep track of how many voxel quads are actually displayed
					LocalVoxelTable.Add(VoxelId.Key, LocalVoxelTable.Num());
					// Add voxel materials to mesh
					const auto MaterialId = VoxelId.Value;
					const auto VoxelType = VoxelGenerator->GetVoxelTypeById(VoxelId.Key);

					MeshActor->ProceduralMeshComponent->SetMaterial(MaterialId, VoxelType.Value.Material);
				}
			}
		}

		int32  SectionIndex = LocalVoxelTable[VoxelId.Key];

		AsyncTask(ENamedThreads::GameThread, [MeshActor, Vertices, Triangles, Normals, SectionIndex, UV0, Tangents]()
		{
			if(MeshActor.IsValid() &&  Vertices.IsValid() && Triangles.IsValid()){
				   MeshActor->ProceduralMeshComponent->CreateMeshSection_LinearColor(SectionIndex, *Vertices.Get(), *Triangles.Get(), *Normals.Get(), *UV0.Get(), TArray<FLinearColor>(), *Tangents.Get(), true);
			   }
		});
		
		GlobalVertexCount += Vertices->Num();
	}

	if (!MeshVars.ChunkParams.OriginalChunk.IsValid() || LocalVoxelTable.IsEmpty())
	{
		return;
	}

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	const FString MapName = GetWorld()->GetMapName();
	FVoxelMeshingProfilingLogger::LogGeneratedVertices(MapName, GlobalVertexCount);
#endif

	MeshVars.ChunkParams.OriginalChunk->bHasMesh = true;
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
