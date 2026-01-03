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
	if (IsValid(this))
	{
		VoxelGenerator = VoxelGeneratorBase;
	}
}

void AChunkActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearMesh();
	Super::EndPlay(EndPlayReason);
}

void AChunkActor::AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor,
	const TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>>& LocalVoxelTable) const
{
	for (const auto LocalVoxelType : LocalVoxelTable)
	{
		// Voxel tables are necessary to map voxel rows from voxel table to mesh sections of procedural mesh
		auto ProcMeshVarsPtr = LocalVoxelType.Value;
		
		const auto VoxelRow = VoxelGenerator->GetVoxelTableRow(LocalVoxelType.Key);

		AsyncTask(ENamedThreads::GameThread, [MeshActor, ProcMeshVarsPtr, VoxelRow]()
		{
			
			const FProcMeshSectionVars& QuadMeshSection = *ProcMeshVarsPtr;
			MeshActor->ProceduralMeshComponent->SetMaterial(QuadMeshSection.MeshSectionId, VoxelRow.Value.Material);

			MeshActor->ProceduralMeshComponent->ClearMeshSection(QuadMeshSection.MeshSectionId);
			// Add voxel materials to mesh
			MeshActor->ProceduralMeshComponent->CreateMeshSection_LinearColor(
				QuadMeshSection.MeshSectionId, QuadMeshSection.Vertices, QuadMeshSection.Triangles, QuadMeshSection.Normals,
				QuadMeshSection.UV0, TArray<FLinearColor>(),
				QuadMeshSection.Tangents, true);
		});
	}
}

void AChunkActor::GenerateMesh(FMesherVariables& MeshVars, TArray<FRLEVoxelEdit>& VoxelEdits, bool ShowBorders) const
{
	auto& VoxelMesher = MeshVars.OriginalChunk->VoxelMesher;
	if (VoxelMesher->bEnableVoxelMeshing)
	{
		//TODO: move
			
	#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Total - Mesh generation - RDM Meshing")
	#endif
		
		const uint32 VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();
		
		// TODO: rewrite this
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces;
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT> SideMeshers;
		
		for (int i = 0; i < CHUNK_FACE_COUNT; i++)
		{
			auto& SideChunk= MeshVars.SideChunks[i];
			if (SideChunk != nullptr)
			{
				SideMeshers[i] = TStrongObjectPtr<UVoxelMesherBase>(SideChunk->VoxelMesher);
			}
			
			SideFaces[i] = MakeShared<TArray<FVirtualVoxelFace>>();
			SideFaces[i]->Reserve(VoxelLayer);
		}
		
		VoxelMesher->PreallocateArrays(MeshVars.VirtualFaces,SideFaces);

		
		VoxelMesher->GenerateMesh(MeshVars.VirtualFaces,
		                          MeshVars.LocalVoxelTable,
		                          MeshVars.ChunkMeshData,VoxelEdits, SideFaces,
		                          SideMeshers, ShowBorders);
		
		AddMeshToActor(MeshVars.OriginalChunk->ChunkMeshActor,
					   MeshVars.LocalVoxelTable);
		
		MeshVars.LocalVoxelTable.Empty();
	}
}
