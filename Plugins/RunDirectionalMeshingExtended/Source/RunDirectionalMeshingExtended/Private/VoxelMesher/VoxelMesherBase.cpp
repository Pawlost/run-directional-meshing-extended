#include "VoxelMesher/VoxelMesherBase.h"

#include "Log/VoxelMeshingProfilingLogger.h"
#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Grid/VoxelGrid.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"

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
	UpdateFaceParams(FaceTemplates[EFaceDirection::Front], FIntVector(1, 0, 0),
					 FIntVector(0, 0, 0),
					FIntVector(0, -1, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Back], FIntVector(-1, 0, 0),
				FIntVector(ChunkDimension - 1, 0, 0),
				FIntVector(0, -1, 0));

	//Axis Y
	UpdateFaceParams(FaceTemplates[EFaceDirection::Right], FIntVector(0, 1, 0),
					 FIntVector(0, 0, 0),
					 FIntVector(-1, 0, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Left],FIntVector(0, -1, 0),
					 FIntVector(0, ChunkDimension - 1, 0),
					 FIntVector(-1, 0, 0));

	// Axis Z
	UpdateFaceParams(FaceTemplates[EFaceDirection::Bottom], FIntVector(0, 0, -1),
	                 FIntVector(0, 0, ChunkDimension - 1),
	                 FIntVector(0, -1, 0));

	UpdateFaceParams(FaceTemplates[EFaceDirection::Top], FIntVector(0, 0, 1),
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

//TODO: delete
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

void UVoxelMesherBase::PreallocateArrays(FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation preallocation")
#endif
	
	auto VoxelTypeCount = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable.Num();
	auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();

	// TODO: rewrite, keep preallocation
	MeshVars.QuadMeshSectionArray = nullptr;

	
	if (MeshVars.QuadMeshSectionArray == nullptr)
	{
		MeshVars.QuadMeshSectionArray = MakeShared<TArray<FProcMeshSectionVars>>();
	}

	for (int t = 0; t < 10; t++)
	{
		auto& QuadMeshSectionArray = *MeshVars.QuadMeshSectionArray;
		if (QuadMeshSectionArray.IsValidIndex(t))
		{
			QuadMeshSectionArray[t].EmptyValues();
		}else
		{
			QuadMeshSectionArray.Emplace(VoxelGenerator->GetVoxelCountPerChunk());
		}
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
		const uint32 EstimatedVoxels = VoxelGenerator->GetVoxelCountPerChunkDimension() * VoxelTable.Num();
		FaceArray->Reserve(EstimatedVoxels);
	}
}

void UVoxelMesherBase::GenerateProcMesh(const FMesherVariables& MeshVars, TMap<uint32, uint32> LocalVoxelTable) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Creating Actor - RunDirectionalMeshing from VoxelGrid generation")
#endif

	auto MeshActor = MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor;

	for (auto LocalVoxelType : LocalVoxelTable)
	{
		auto SectionId = LocalVoxelType.Value;
		const auto VoxelType = VoxelGenerator->GetVoxelTypeById(LocalVoxelType.Key);
		auto QuadMeshSectionArray = MeshVars.QuadMeshSectionArray;

		AsyncTask(ENamedThreads::GameThread, [MeshActor, QuadMeshSectionArray, SectionId, VoxelType]()
		{
			MeshActor->ProceduralMeshComponent->SetMaterial(SectionId, VoxelType.Value.Material);
			FProcMeshSectionVars& QuadMeshSection = (*QuadMeshSectionArray)[SectionId];

			MeshActor->ProceduralMeshComponent->ClearMeshSection(SectionId);
			// Add voxel materials to mesh
			MeshActor->ProceduralMeshComponent->CreateMeshSection_LinearColor(
				SectionId, QuadMeshSection.Vertices, QuadMeshSection.Triangles, QuadMeshSection.Normals,
				QuadMeshSection.UV0, TArray<FLinearColor>(),
				QuadMeshSection.Tangents, true);
		});
	}

	MeshVars.ChunkParams.OriginalChunk->bHasMesh = !LocalVoxelTable.IsEmpty();
}

void UVoxelMesherBase::ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray, const FVoxelFace& Face,
                                             TMap<uint32, uint32>& LocalVoxelTable, int FaceIndex) const
{
	const double VoxelSize = VoxelGenerator->GetVoxelSize();
	
	const auto VoxelId = Face.Voxel.VoxelId;
	const int32 SectionId = LocalVoxelTable.FindOrAdd(VoxelId, LocalVoxelTable.Num());

	auto& QuadSection = QuadMeshSectionArray[SectionId];
	auto [Normal, Tangent] = FaceNormalsAndTangents[FaceIndex];
	auto& TriangleIndex = QuadSection.GlobalTriangleIndex;

	// Create quad from 2 triangles
	QuadSection.Vertices.Push(Face.GetFinalStartVertexDown(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalEndVertexDown(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalEndVertexUp(VoxelSize));
	QuadSection.Vertices.Push(Face.GetFinalStartVertexUp(VoxelSize));

	QuadSection.Triangles.Push(TriangleIndex);
	QuadSection.Triangles.Push(TriangleIndex + 1);
	QuadSection.Triangles.Push(TriangleIndex + 2);
	QuadSection.Triangles.Push(TriangleIndex + 2);
	QuadSection.Triangles.Push(TriangleIndex + 3);
	QuadSection.Triangles.Push(TriangleIndex);

	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);
	QuadSection.Normals.Push(Normal);

	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);
	QuadSection.Tangents.Push(Tangent);

	QuadSection.UV0.Push(FVector2D(0, 0));
	QuadSection.UV0.Push(FVector2D(1, 0));
	QuadSection.UV0.Push(FVector2D(1, 1));
	QuadSection.UV0.Push(FVector2D(0, 1));

	TriangleIndex += 4;
}

void UVoxelMesherBase::DirectionalGreedyMerge(const FMesherVariables& MeshVars,
														TMap<uint32, uint32>& LocalVoxelTable,
														const FStaticMergeData& MergeData,
														TArray<FVoxelFace>& FaceContainer) const
{
	int FaceIndex = FaceContainer.Num() - 2;

	// Iterate from last face
	for (int32 i = FaceIndex; i >= 0; i--)
	{
		FVoxelFace& NextFace = FaceContainer[i + 1];

		// Elements are removed and it must be updated
		int BackTrackIndex = i;

		FVoxelFace* Face = &FaceContainer[BackTrackIndex];
		while (!MergeData.GreedyMerge(*Face, NextFace))
		{
			BackTrackIndex--;

			if (BackTrackIndex == -1 || MergeData.RowBorderCondition(*Face, NextFace))
			{
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NextFace, LocalVoxelTable, MergeData.FaceDirection);
				break;
			}

			Face = &FaceContainer[BackTrackIndex];
		}

		FaceContainer.Pop(EAllowShrinking::No);
	}

	FaceIndex = FaceContainer.Num();
	for (int i = 0; i < FaceIndex; i++)
	{
		const FVoxelFace& Face = FaceContainer[i];
		ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, Face, LocalVoxelTable, MergeData.FaceDirection);
	}

	FaceContainer.Empty();
}

void UVoxelMesherBase::AddFace(const FStaticMergeData& FaceMeshingData, const FVoxelFace& NewFace, const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces)
{
	// Generate new face with coordinates
	if (!ChunkFaces->IsEmpty())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		FVoxelFace& PrevFace = ChunkFaces->Last();

		if (FaceMeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
		{
			// Return when new face was merged
			return;
		}
	}

	ChunkFaces->Push(NewFace);
}