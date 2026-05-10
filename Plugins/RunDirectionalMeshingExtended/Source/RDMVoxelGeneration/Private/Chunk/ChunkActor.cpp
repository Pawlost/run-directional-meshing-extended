#include "Chunk/ChunkActor.h"

#include "VoxelGeneratorBase.h"
#include "VirtualChunk/ChunkBorderContext.h"
#include "VirtualChunk/VirtualChunkBase.h"

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

void AChunkActor::AddMeshToActor(const FVoxelMesh& LocalVoxelTable) const
{
	for (const auto LocalVoxelType : LocalVoxelTable.VoxelTable)
	{
		// Voxel tables are necessary to map voxel rows from voxel table to mesh sections of procedural mesh
		auto ProcMeshVarsPtr = LocalVoxelType.Value;
		
		const auto VoxelRow = VoxelGenerator->GetVoxelTableRow(LocalVoxelType.Key);

		AsyncTask(ENamedThreads::GameThread, [this, ProcMeshVarsPtr, VoxelRow]()
		{
			const FProcMeshSectionVars& QuadMeshSection = *ProcMeshVarsPtr;
			ProceduralMeshComponent->SetMaterial(QuadMeshSection.MeshSectionId, VoxelRow.Value.Material);

			ProceduralMeshComponent->ClearMeshSection(QuadMeshSection.MeshSectionId);
			// Add voxel materials to mesh
			ProceduralMeshComponent->CreateMeshSection_LinearColor(
				QuadMeshSection.MeshSectionId, QuadMeshSection.Vertices, QuadMeshSection.Triangles, QuadMeshSection.Normals,
				QuadMeshSection.UV0, TArray<FLinearColor>(),
				QuadMeshSection.Tangents, true);
		});
	}
}