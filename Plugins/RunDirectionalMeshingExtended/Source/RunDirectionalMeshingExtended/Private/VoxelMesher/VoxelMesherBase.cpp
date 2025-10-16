﻿#include "VoxelMesher/VoxelMesherBase.h"

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
	// TODO: fix measuring
	/*for (const auto OpaqueVoxels : Chunk.ChunkVoxelIdTable)
	{
		OpaqueVoxelCount += OpaqueVoxels.Value;
	}*/
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
	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
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

void UVoxelMesherBase::PreallocateArrays(FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation preallocation")
#endif
	
	auto ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	auto ChunkLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();

	// TODO: rewrite, keep preallocation
	MeshVars.ChunkMeshData = nullptr;

	if (MeshVars.ChunkMeshData == nullptr)
	{
		MeshVars.ChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	}

	MeshVars.BorderChunkMeshData = nullptr;
	if (MeshVars.BorderChunkMeshData == nullptr)
	{
		MeshVars.BorderChunkMeshData = MakeShared<TArray<FProcMeshSectionVars>>();
	}

	for (int t = 0; t < 10; t++)
	{
		auto& ChunkMeshData = *MeshVars.ChunkMeshData;
		if (ChunkMeshData.IsValidIndex(t))
		{
			ChunkMeshData[t].EmptyValues();
		}else
		{
			ChunkMeshData.Emplace(VoxelGenerator->GetVoxelCountPerChunk());
		}

		auto& BorderChunkMeshData = *MeshVars.BorderChunkMeshData;
		if (BorderChunkMeshData.IsValidIndex(t))
		{
			BorderChunkMeshData[t].EmptyValues();
		}else
		{
			BorderChunkMeshData.Emplace(VoxelGenerator->GetVoxelCountPerChunk());
		}
	}
	
	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		auto FaceArray = MeshVars.VirtualFaces[f];
		if (FaceArray == nullptr || !FaceArray.IsValid())
		{
			// In case voxel table is not available this code needs to be rewritten to add local voxels id dynamically during voxel grid traversal
			FaceArray = MakeShared<TArray<TArray<FVirtualVoxelFace>>>();
			MeshVars.VirtualFaces[f] = FaceArray;
		}
		else
		{
			// If array was pulled from a pool, just empty it 
			FaceArray->Empty();
		}

		MeshVars.VirtualFaces[f]->SetNum(ChunkDimension);
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			(*MeshVars.VirtualFaces[f])[y].Reserve(ChunkLayer);
		}
	}
}

void UVoxelMesherBase::GenerateProcMesh(const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Creating Actor - RunDirectionalMeshing from VoxelGrid generation")
#endif

	AddMeshToActor(MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor, MeshVars.ChunkMeshData, MeshVars.LocalVoxelTable);
	AddMeshToActor(MeshVars.ChunkParams.OriginalChunk->BorderChunkMeshActor, MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable);
}

void UVoxelMesherBase::AddMeshToActor(TWeakObjectPtr<AChunkActor> MeshActor, TSharedPtr<TArray<FProcMeshSectionVars>> ChunkMeshData,
	const TMap<int32, uint32>& LocalVoxelTable) const
{
	for (const auto LocalVoxelType : LocalVoxelTable)
	{
		auto SectionId = LocalVoxelType.Value;

		if (!IsValid(VoxelGenerator))
		{
			return;
		}

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

void UVoxelMesherBase::ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray, TMap<int32, uint32>& LocalVoxelTable, const FVirtualVoxelFace& Face,
                                              const int FaceIndex) const
{
	const double VoxelSize = VoxelGenerator->GetVoxelSize();
	
	const auto VoxelId = Face.Voxel.VoxelId;
	// TODO: remove
	check(VoxelId != 0);
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

void UVoxelMesherBase::DirectionalGreedyMerge(TArray<FProcMeshSectionVars>& ChunkMeshData, TMap<int32, uint32>& LocalVoxelTable,
														const FStaticMergeData& MergeData,
														TArray<FVirtualVoxelFace>& FaceContainer) const
{
	auto FirstQueue = TQueue<FVirtualVoxelFace>();
	TQueue<FVirtualVoxelFace> SecondQueue;
	TQueue<FVirtualVoxelFace>* ActiveQueue = &FirstQueue;
	TQueue<FVirtualVoxelFace>* PassiveQueue = &SecondQueue;
	
	FVirtualVoxelFace PrevFace;
	const int FaceIndex = FaceContainer.Num() - 1;
	
	// Iterate from last face
	for (int32 i = FaceIndex; i >= 0; i--)
	{
		PrevFace = FaceContainer.Pop(EAllowShrinking::No);

		if (const FVirtualVoxelFace* QueueFacePeek = ActiveQueue->Peek();
			QueueFacePeek == nullptr || MergeData.HeightCondition(*QueueFacePeek, PrevFace))
		{
			ActiveQueue->Enqueue(PrevFace);
			continue;
		}
		
		FVirtualVoxelFace QueueFace;

		while (ActiveQueue->Dequeue(QueueFace))
		{
			if (MergeData.MergeFailCondition(QueueFace, PrevFace))
			{
				ConvertFaceToProcMesh(ChunkMeshData, LocalVoxelTable, QueueFace, MergeData.FaceDirection);
				continue;
			}
			
			// Attempt greedy merge
			if(!MergeData.GreedyMerge(PrevFace, QueueFace))
			{
				PassiveQueue->Enqueue(QueueFace);
			}
		}

		PassiveQueue->Enqueue(PrevFace);
		
		Swap(PassiveQueue, ActiveQueue);
	}

	while (ActiveQueue->Dequeue(PrevFace))
	{
		ConvertFaceToProcMesh(ChunkMeshData,LocalVoxelTable, PrevFace, MergeData.FaceDirection);
	}

	while (PassiveQueue->Dequeue(PrevFace))
	{
		ConvertFaceToProcMesh(ChunkMeshData,LocalVoxelTable, PrevFace, MergeData.FaceDirection);
	}
}

void UVoxelMesherBase::AddFace(const FStaticMergeData& FaceMeshingData, const FVirtualVoxelFace& NewFace, TArray<FVirtualVoxelFace>& ChunkFaces)
{
	// TODO: remove
	check(NewFace.Voxel.VoxelId != 0);
	// Generate new face with coordinates
	if (!ChunkFaces.IsEmpty())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		FVirtualVoxelFace& PrevFace = ChunkFaces.Last();

		if (FaceMeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
		{
			// Return when new face was merged
			return;
		}
	}

	ChunkFaces.Push(NewFace);
}