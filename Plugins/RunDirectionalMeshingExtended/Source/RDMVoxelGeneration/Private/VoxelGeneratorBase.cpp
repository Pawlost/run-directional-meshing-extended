#include "VoxelGeneratorBase.h"

#include "Chunk/MesherVariables.h"
#include "VoxelMesher/VoxelMesherBase.h"
#include "VoxelMesher/MeshingUtils/FaceDirection.h"

void UVoxelGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
	
	checkf(VoxelMesherBlueprint, TEXT("Mesher blueprint must be set"));
	if (VoxelMesherBlueprint)
	{
		// Register mesher
		VoxelMesher = NewObject<UVoxelMesherBase>(this, VoxelMesherBlueprint);

		if (VoxelMesher)
		{
			VoxelMesher->SetVoxelGenerator(this);
			VoxelMesher->RegisterComponent();
		}
	}
}

void UVoxelGeneratorBase::GenerateVoxels(FChunk& Chunk)
{
	FScopeLock Lock(&Mutex);
	TArray<FVoxel> VoxelModelArray;
	AddVoxels(Chunk, VoxelModelArray);
	VoxelMesher->CompressVoxelModel(Chunk.VoxelModel, VoxelModelArray);
}

void UVoxelGeneratorBase::GenerateProcMesh(const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Creating Actor - RunDirectionalMeshing from VoxelGrid generation")
#endif

	AddMeshToActor(MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor, MeshVars.ChunkMeshData,
	               MeshVars.LocalVoxelTable);
	AddMeshToActor(MeshVars.ChunkParams.OriginalChunk->BorderChunkMeshActor, MeshVars.BorderChunkMeshData,
	               MeshVars.BorderLocalVoxelTable);
}

void UVoxelGeneratorBase::AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor,
                                         TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData,
                                         const TMap<int32, uint32>& LocalVoxelTable) const
{
	for (const auto LocalVoxelType : LocalVoxelTable)
	{
		auto SectionId = LocalVoxelType.Value;

		const auto Voxel = FVoxel(LocalVoxelType.Key);
		const auto VoxelRow = GetVoxelTableRow(Voxel);

		AsyncTask(ENamedThreads::GameThread, [MeshActor, ChunkMeshData, SectionId, VoxelRow]()
		{
			MeshActor->ProceduralMeshComponent->SetMaterial(SectionId, VoxelRow.Value.Material);
			const FProcMeshSectionVars& QuadMeshSection = (*ChunkMeshData)[SectionId];

			MeshActor->ProceduralMeshComponent->ClearMeshSection(SectionId);
			// Add voxel materials to mesh
			MeshActor->ProceduralMeshComponent->CreateMeshSection_LinearColor(
				SectionId, QuadMeshSection.Vertices, QuadMeshSection.Triangles, QuadMeshSection.Normals,
				QuadMeshSection.UV0, TArray<FLinearColor>(),
				QuadMeshSection.Tangents, true);
		});
	}
}

void UVoxelGeneratorBase::GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelEdit>& VoxelChanges) const
{
	if (bEnableVoxelMeshing)
	{
		const uint32 VoxelLayer = GetVoxelCountPerVoxelPlane();

		TStaticArray<bool*, CHUNK_FACE_COUNT> IsBorderSampled;

		auto& BorderChunks = MeshVars.ChunkParams.OriginalChunk->BorderChunks;

		// Allocate Borders, after creating change
		// TODO: move
		
		TSharedPtr<TArray<FRLEVoxel>> SampledBorderChunks[CHUNK_FACE_COUNT];
		
		for (int d = 0; d < CHUNK_FACE_COUNT; d++)
		{
			const auto& FaceTemplate = VoxelMesher->FaceTemplates[d];

			if (BorderChunks[d] == nullptr)
			{
				auto SideChunk = MeshVars.ChunkParams.SideChunks[FaceTemplate.StaticMeshingData.FaceDirection];
				if (SideChunk != nullptr && SideChunk->VoxelModel != nullptr)
				{
					// TODO: move to chunk spawning
					auto& BorderChunkPtr = SideChunk->BorderChunks[FaceTemplate.StaticMeshingData.InverseFaceDirection];

					if (BorderChunkPtr == nullptr)
					{
						BorderChunkPtr = MakeShared<FBorderChunk>(VoxelLayer);
					}

					BorderChunks[d] = BorderChunkPtr;
				}
				else
				{
					BorderChunks[d] = MakeShared<FBorderChunk>(VoxelLayer);
				}
			}

			TSharedPtr<TArray<FRLEVoxel>> BorderChunkSamples = nullptr;
			bool* IsSampled;

			if (FaceTemplate.StaticMeshingData.IsInverseDirection)
			{
				auto BorderChunk = BorderChunks[d];
				BorderChunkSamples = BorderChunk->InversedBorderVoxelSamples;
				IsSampled = &BorderChunk->IsInverseSampled;

				//TODO: try better preallocation
				BorderChunk->IsInverseSampled = false;
				BorderChunk->InversedBorderVoxelSamples->Reset();
				BorderChunk->InversedBorderVoxelSamples->SetNum(VoxelLayer);
			}
			else
			{
				auto BorderChunk = BorderChunks[d];
				BorderChunkSamples = BorderChunk->BorderVoxelSamples;
				IsSampled = &BorderChunk->IsSampled;

				BorderChunk->IsSampled = false;
				BorderChunk->BorderVoxelSamples->Reset();
				BorderChunk->BorderVoxelSamples->SetNum(VoxelLayer);
			}

			BorderChunks[d]->IsGenerated = false;
			SampledBorderChunks[d] = BorderChunkSamples;
			IsBorderSampled[d] = IsSampled;
		}

		VoxelMesher->GenerateMesh(MeshVars.ChunkParams.OriginalChunk->VoxelModel, MeshVars.VirtualFaces,
		                          MeshVars.LocalVoxelTable, MeshVars.BorderLocalVoxelTable,
		                          MeshVars.ChunkMeshData, MeshVars.BorderChunkMeshData, VoxelChanges, BorderChunks,
		                          SampledBorderChunks, IsBorderSampled,
		                          MeshVars.ChunkParams.ShowBorders);

		GenerateProcMesh(MeshVars);
	}
}
