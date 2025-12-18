#include "Chunk/ChunkActor.h"

#include "VoxelGeneratorBase.h"

AChunkActor::AChunkActor() 
{
	// Register and set RealTimeMeshComponent which will render and store generated mesh.
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RealtimeMeshComponent"));
	SetRootComponent(ProceduralMeshComponent);
}

bool AChunkActor::HasMesh() const
{
	return IsValid(ProceduralMeshComponent) && ProceduralMeshComponent->GetNumSections() != 0;
}

void AChunkActor::ClearMesh() const
{
	if (!IsValid(this))
	{
		return;
	}
	
	ProceduralMeshComponent->ClearAllMeshSections();
}

void AChunkActor::SetVoxelGenerator(const TObjectPtr<UVoxelGeneratorBase>& VoxelGeneratorBase)
{
	VoxelGenerator = VoxelGeneratorBase;
}

void AChunkActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearMesh();
	Super::EndPlay(EndPlayReason);
}

void AChunkActor::AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor,
										 TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData,
										 const TMap<int32, uint32>& LocalVoxelTable) const
{
	for (const auto LocalVoxelType : LocalVoxelTable)
	{
		auto SectionId = LocalVoxelType.Value;

		const auto Voxel = FVoxel(LocalVoxelType.Key);
		const auto VoxelRow = VoxelGenerator->GetVoxelTableRow(Voxel);

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

void AChunkActor::GenerateMesh(FMesherVariables& MeshVars, TArray<FRLEVoxelEdit>& VoxelEdits) const
{
	auto& VoxelMesher = MeshVars.OriginalChunk->VoxelMesher;
	if (VoxelMesher->bEnableVoxelMeshing)
	{
		//TODO: move
			
	#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Creating Actor - RunDirectionalMeshing from VoxelGrid generation")
	#endif
		
		const uint32 VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();
		UVoxelMesherBase::FBorderSamples BorderSamples(VoxelLayer);
		
		// TODO: rewrite this
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces;
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT> SideMeshers;
		
		for (int i = 0; i < CHUNK_FACE_COUNT; i++)
		{
			auto& SideChunk= MeshVars.SideChunks[i];
			if (SideChunk != nullptr)
			{
				SideMeshers = TStrongObjectPtr<UVoxelMesherBase>(SideChunk->VoxelMesher);
			}
			
			SideFaces[i] = MakeShared<TArray<FVirtualVoxelFace>>();
		}
		
		VoxelMesher->PreallocateArrays(MeshVars.VirtualFaces,SideFaces, MeshVars.ChunkMeshData, MeshVars.BorderChunkMeshData);

		VoxelMesher->GenerateMesh(MeshVars.OriginalChunk->VoxelModel, MeshVars.VirtualFaces,
		                          MeshVars.LocalVoxelTable,
		                          MeshVars.ChunkMeshData,VoxelEdits, SideFaces,
		                          SideMeshers, BorderSamples);
		
		AddMeshToActor(MeshVars.OriginalChunk->ChunkMeshActor, MeshVars.ChunkMeshData,
					   MeshVars.LocalVoxelTable);
		
		
		// TODO: rewrite
		/*bool ShowBorders = true;
		
		if (ShowBorders){
		
			UVoxelMesherBase::FBorderSamples InverseBorderSamples(VoxelLayer); 
			
			for (uint8 d = 0; d < CHUNK_FACE_COUNT; d++)
			{
				const auto& FaceTemplate = VoxelMesher->FaceTemplates[d];
				auto LeftVoxelModel = MeshVars.SideChunks[FaceTemplate.StaticMeshingData.FaceDirection];
		
				if(LeftVoxelModel != nullptr)
				{
					const auto LeftVoxelGridPtr = Cast<URLEVoxelGrid>(LeftVoxelModel->VoxelModel.Get());
					VoxelMesher->SampleLeftChunkBorder(InverseBorderSamples, LeftVoxelGridPtr->RLEVoxelGrid);
				}
				
				auto BorderSample = BorderSamples.BorderSamples[d];
				auto InverseBorderSample = InverseBorderSamples.BorderSamples[d];
				VoxelMesher->BorderGeneration(MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable, BorderSample, InverseBorderSample, static_cast<EFaceDirection>(d));
				AddMeshToActor(MeshVars.OriginalChunk->BorderChunkMeshActor[d], MeshVars.BorderChunkMeshData,
						   MeshVars.BorderLocalVoxelTable);
			}
		}*/
	}
}
