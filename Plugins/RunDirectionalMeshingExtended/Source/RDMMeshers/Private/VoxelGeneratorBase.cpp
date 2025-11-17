#include "VoxelGeneratorBase.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UVoxelGeneratorBase::BeginPlay()
{
	// Calculate the total number of voxels in a chunk along each axis
	VoxelCountY = VoxelCountPerChunkDimension;
	ChunkSpacing = VoxelCountY * VoxelSize;
	VoxelCountYZ = VoxelCountY * VoxelCountY;
	VoxelCountXYZ = VoxelCountYZ * VoxelCountY;

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

void UVoxelGeneratorBase::ChangeKnownVoxelAtIndex(TArray<FVoxel>& VoxelGrid,
                                                  const uint32& Index,
                                                  const FVoxel& Voxel)
{
	// NOTICE: Code here is optimized because voxel grid generation is not main topic of this bachelor's thesis 
	FScopeLock Lock(&Mutex);
	// Replace the voxel
	VoxelGrid[Index] = Voxel;
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const int32 X, const int32 Y, const int32 Z) const
{
	return Y + (Z * VoxelCountY) + (X * VoxelCountYZ);
}

uint32 UVoxelGeneratorBase::CalculateVoxelIndex(const FIntVector& VoxelPosition) const
{
	return CalculateVoxelIndex(VoxelPosition.X, VoxelPosition.Y, VoxelPosition.Z);
}

double UVoxelGeneratorBase::GetChunkSpacing() const
{
	return ChunkSpacing;
}

double UVoxelGeneratorBase::GetVoxelSize() const
{
	return VoxelSize;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerVoxelLine() const
{
	return VoxelCountPerChunkDimension;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerVoxelPlane() const
{
	return VoxelCountYZ;
}

uint32 UVoxelGeneratorBase::GetVoxelCountPerChunk() const
{
	return VoxelCountXYZ;
}

void UVoxelGeneratorBase::GenerateMesh(FMesherVariables& MesherVariables, TArray<FVoxelEdit>& VoxelChanges) const
{
	const uint32 VoxelLayer = GetVoxelCountPerVoxelPlane();
	
	bool* IsBorderSampled[CHUNK_FACE_COUNT];

	auto& BorderChunks = MeshVars.ChunkParams.OriginalChunk->BorderChunks;

	// Allocate Borders, after creating change
	// TODO: move
	for (int d = 0; d < CHUNK_FACE_COUNT; d++)
	{
		const auto& FaceTemplate = FaceTemplates[d];

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
		IndexParams.SampledBorderChunks[d] = BorderChunkSamples;
		IsBorderSampled[d] = IsSampled;
	}

	
	if (bEnableVoxelMeshing)
	{
		VoxelMesher->GenerateMesh(MesherVariables, VoxelChanges);
	}
	
	
	for (int i = 0; i < CHUNK_FACE_COUNT; ++i)
	{
		auto IsSampled = IsBorderSampled[i];
		if (IsSampled != nullptr)
		{
			*(IsSampled) = true;
		}
	}

	BorderGeneration(MeshVars, BorderChunks);

	GenerateProcMesh(MeshVars);
}

double UVoxelGeneratorBase::GetHighestElevationAtLocation(const FVector& Location)
{
	return GetChunkSpacing();
}