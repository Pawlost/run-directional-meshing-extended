#include "VoxelMesher/VoxelMesherBase.h"

#include "BaseVoxelData.h"
#include "VoxelMesher/MeshingUtils/ProcMeshSectionVars.h"
#include "VoxelModel/VoxelGrid.h"

void UVoxelMesherBase::SetVoxelGenerator(const TObjectPtr<UBaseVoxelData>& VoxelGeneratorBase)
{
	this->VoxelData = VoxelGeneratorBase;
	UpdateAllFacesParams();
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
	const auto ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
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

	UpdateFaceParams(FaceTemplates[EFaceDirection::Left], FIntVector(0, -1, 0),
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
	Face.ForwardVoxelIndex = VoxelData->CalculateVoxelIndex(ForwardVoxelIndexVector);
	Face.PreviousVoxelIndex = VoxelData->CalculateVoxelIndex(PreviousVoxelIndexVector);
	Face.ChunkBorderIndex = VoxelData->CalculateVoxelIndex(ChunkBorderIndexVector);
}

void UVoxelMesherBase::PreallocateArrays(
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
	TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Mesh generation preallocation")
#endif

	auto ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	auto ChunkLayer = VoxelData->GetVoxelCountPerVoxelPlane();
	
	for (uint8 f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		auto FaceArray = VirtualFaces[f];

		SideFaces[f] = MakeShared<TArray<FVirtualVoxelFace>>();
		SideFaces[f]->Reserve(ChunkLayer);

		if (FaceArray == nullptr || !FaceArray.IsValid())
		{
			VirtualFaces[f] = MakeShared<TArray<TArray<FVirtualVoxelFace>>>();
		}
		else
		{
			// If array was pulled from a pool, just empty it 
			VirtualFaces[f]->Reset();
		}

		VirtualFaces[f]->SetNum(ChunkDimension);
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			(*VirtualFaces[f])[y].Reserve(ChunkLayer);
		}
	}
}

void UVoxelMesherBase::ConvertFaceToProcMesh(TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>>& LocalVoxelTable, const FVirtualVoxelFace& Face,
                                             const int FaceIndex) const
{
	const double VoxelSize = VoxelData->GetVoxelSize();

	const auto& Voxel = Face.Voxel;
	// TODO: remove
	check(Voxel.VoxelId != 0);

	const TSharedPtr<FProcMeshSectionVars>* ProcMeshVars = LocalVoxelTable.Find(Voxel);

	if (!ProcMeshVars)
	{
		ProcMeshVars = &LocalVoxelTable.Add(Voxel, MakeShared<FProcMeshSectionVars>(VoxelData->GetVoxelCountPerChunk(), LocalVoxelTable.Num()));
	}
	
	auto& QuadSection = **ProcMeshVars;
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

void UVoxelMesherBase::DirectionalGreedyMerge(TArray<FVirtualVoxelFace>& FirstArray,
                                              TArray<FVirtualVoxelFace>& SecondArray,
                                              TMap<FVoxel, TSharedPtr<FProcMeshSectionVars>>& LocalVoxelTable,
                                              const FStaticMergeData& MergeData,
                                              TArray<FVirtualVoxelFace>& FaceContainer) const
{
	TArray<FVirtualVoxelFace>* ActiveArray = &FirstArray;
	TArray<FVirtualVoxelFace>* PassiveArray = &SecondArray;

	// Iterate from last face
	for (int32 i = FaceContainer.Num() - 1; i >= 0; i--)
	{
		FVirtualVoxelFace PrevFace = FaceContainer.Pop(EAllowShrinking::No);

		if (ActiveArray->IsEmpty() || MergeData.HeightCondition(ActiveArray->Top(), PrevFace))
		{
			ActiveArray->Push(PrevFace);
		}
		else
		{
			while (!ActiveArray->IsEmpty())
			{
				const FVirtualVoxelFace& PopFace = ActiveArray->Pop(EAllowShrinking::No);
				if (!MergeData.MergeFailCondition(PopFace, PrevFace))
				{
					// Attempt greedy merge
					if (MergeData.GreedyMerge(PrevFace, PopFace))
					{
						PassiveArray->Append(*ActiveArray);
						ActiveArray->Reset();
					}
					else
					{
						PassiveArray->Push(PopFace);
					}
				}
				else
				{
					ConvertFaceToProcMesh(LocalVoxelTable, PopFace, MergeData.FaceDirection);
				}
			}
			
			PassiveArray->Push(PrevFace);
		}

		Swap(PassiveArray, ActiveArray);
	}

	while (!ActiveArray->IsEmpty())
	{
		const FVirtualVoxelFace& PopFace = ActiveArray->Pop(EAllowShrinking::No);
		ConvertFaceToProcMesh(LocalVoxelTable, PopFace, MergeData.FaceDirection);
	}

	while (!PassiveArray->IsEmpty())
	{
		const FVirtualVoxelFace& PopFace = PassiveArray->Pop(EAllowShrinking::No);
		ConvertFaceToProcMesh(LocalVoxelTable, PopFace, MergeData.FaceDirection);
	}
}

void UVoxelMesherBase::AddFace(const FStaticMergeData& FaceMeshingData, const FVirtualVoxelFace& NewFace,
                               TArray<FVirtualVoxelFace>& ChunkFaces)
{
	// TODO: remove
	auto VoxelId = NewFace.Voxel.VoxelId;
	check(VoxelId != 0);
	
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
