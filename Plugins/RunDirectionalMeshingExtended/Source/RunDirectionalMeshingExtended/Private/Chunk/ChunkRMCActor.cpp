#include "Chunk/ChunkRMCActor.h"

AChunkRMCActor::AChunkRMCActor() 
{
	// Register and set RealTimeMeshComponent which will render and store generated mesh.
	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("RealtimeMeshComponent"));
	SetRootComponent(ProceduralMeshComponent);
}

void AChunkRMCActor::ClearMesh() const
{
	if (!IsValid(this))
	{
		return;
	}
	
	ProceduralMeshComponent->ClearAllMeshSections();
}

void AChunkRMCActor::PrepareMesh() const
{
}

void AChunkRMCActor::BeginPlay()
{
	
}

void AChunkRMCActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearMesh();
	Super::EndPlay(EndPlayReason);
}
