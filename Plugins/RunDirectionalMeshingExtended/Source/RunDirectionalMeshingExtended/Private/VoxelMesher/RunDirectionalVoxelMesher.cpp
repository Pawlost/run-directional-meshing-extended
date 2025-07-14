#include "VoxelMesher/RunDirectionalVoxelMesher.h"

#include "Log/VoxelMeshingProfilingLogger.h"
#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "Voxel/Grid/VoxelGrid.h"

void URunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, FVoxelChange* VoxelChange)
{
	if (EmptyActor(MeshVars))
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<UVoxelGrid>(MeshVars.ChunkParams.OriginalChunk->VoxelModel);

	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	const auto& VoxelGrid = *VoxelGridPtr;

	if (VoxelChange != nullptr)
	{
		ChangeVoxelId(VoxelGrid, MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable, *VoxelChange);
	}

	PreallocateArrays(MeshVars);
	FaceGeneration(VoxelGrid, MeshVars);
}

void URunDirectionalVoxelMesher::FaceGeneration(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from VoxelGrid generation")
#endif

	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();

	if (!IsValid(VoxelGenerator))
	{
		return;
	}

	// Local voxel table 
	TMap<uint32, uint32> LocalVoxelTable;
	SIZE_T GlobalVertexCount = 0;
	
	// Traverse through voxel grid
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		// Border is necessary to know if voxels from neighboring chunk are needed.
		const auto bMinBorder = IsMinBorder(x);
		const auto bMaxBorder = IsMaxBorder(x);

		// Get last voxel coordinate which is different for each face
		const auto XAxisIndex = VoxelGenerator->CalculateVoxelIndex(x, 0, 0);
		const auto YAxisIndex = VoxelGenerator->CalculateVoxelIndex(0, x, 0);
		const auto ZAxisIndex = VoxelGenerator->CalculateVoxelIndex(0, 0, x);

		for (uint32 z = 0; z < ChunkDimension; z++)
		{
			for (uint32 y = 0; y < ChunkDimension; y++)
			{
				
				/*
				* Increment run for each chunk axis
				* Coordinates are different in order to create a sorted array of quads/faces.
				* Sorted array is then used in Run Direction Greedy meshing, because it is sorted the array needs to traversed only once.
				* Grid is only read from, it doesn't matter which coordinate is first
				* Each voxel needs to be visited only once per face.
				* Indices are same for face and reversed face.
				*
				* The run direction is from left to right, bottom to top and left to right.
				*/
				IncrementRun(x, y, z, XAxisIndex, bMinBorder, bMaxBorder, BackFaceTemplate, FrontFaceTemplate,
				             MeshVars, VoxelGridObject);
				IncrementRun(y, x, z, YAxisIndex, bMinBorder, bMaxBorder, LeftFaceTemplate, RightFaceTemplate,
				             MeshVars, VoxelGridObject);
				IncrementRun(z, y, x, ZAxisIndex, bMinBorder, bMaxBorder, BottomFaceTemplate, TopFaceTemplate,
				             MeshVars, VoxelGridObject);
			}
		}


		// Directional Greedy Meshing
		DirectionalGreedyMerge(MeshVars, LocalVoxelTable,
		                       FStaticGreedyMergeData::FrontFace);

		DirectionalGreedyMerge(MeshVars,  LocalVoxelTable,
		                        FStaticGreedyMergeData::BackFace);

		DirectionalGreedyMerge(MeshVars,  LocalVoxelTable,
		                       FStaticGreedyMergeData::RightFace);

		DirectionalGreedyMerge(MeshVars,  LocalVoxelTable,
		                        FStaticGreedyMergeData::LeftFace);

		DirectionalGreedyMerge(MeshVars, LocalVoxelTable,
		                       FStaticGreedyMergeData::TopFace);

		DirectionalGreedyMerge(MeshVars,  LocalVoxelTable,
		                       FStaticGreedyMergeData::BottomFace);
	}


	// Iterate through merged faces

	for (uint8 FaceIndex = 0; FaceIndex < CHUNK_FACE_COUNT; FaceIndex++)
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Adding Mesh - RunDirectionalMeshing from VoxelGrid generation")
#endif

		auto FaceContainer = MeshVars.Faces[FaceIndex];

		// Create quad foreach face
		for (auto Face : *FaceContainer)
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, Face, LocalVoxelTable, FaceIndex);
		}
	}

	GenerateProcMesh(MeshVars, LocalVoxelTable);

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

void URunDirectionalVoxelMesher::GenerateProcMesh(const FMesherVariables& MeshVars,
                                                  TMap<uint32, uint32> LocalVoxelTable) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Creating Actor - RunDirectionalMeshing from VoxelGrid generation")
#endif

	auto MeshActor = MeshVars.ChunkParams.OriginalChunk->ChunkMeshActor;

	for (auto LocalVoxelType : LocalVoxelTable)
	{
		auto SectionId = LocalVoxelType.Value;
		const auto VoxelType = VoxelGenerator->GetVoxelTypeById(LocalVoxelType.Key);
		MeshActor->ProceduralMeshComponent->SetMaterial(SectionId, VoxelType.Value.Material);
		auto QuadMeshSectionArray = MeshVars.QuadMeshSectionArray;

		AsyncTask(ENamedThreads::GameThread, [MeshActor, QuadMeshSectionArray, SectionId]()
		{
			FProcMeshSectionVars& QuadMeshSection = (*QuadMeshSectionArray)[SectionId];

			MeshActor->ProceduralMeshComponent->ClearMeshSection(SectionId);
			// Add voxel materials to mesh
			MeshActor->ProceduralMeshComponent->CreateMeshSection_LinearColor(
				SectionId, QuadMeshSection.Vertices, QuadMeshSection.Triangles, QuadMeshSection.Normals,
				QuadMeshSection.UV0, TArray<FLinearColor>(),
				QuadMeshSection.Tangents, true);
		});
	}
}

void URunDirectionalVoxelMesher::DirectionalGreedyMerge(const FMesherVariables& MeshVars,
                                                        TMap<uint32, uint32>& LocalVoxelTable,
                                                        const FStaticGreedyMergeData& GreedyMergeData) const
{
	const auto FaceDirectionIndex = static_cast<uint8>(GreedyMergeData.FaceSide);

	auto& FaceContainer = *MeshVars.Faces[FaceDirectionIndex];
	int FaceIndex = FaceContainer.Num() - 2;

	// Iterate from last face
	for (int32 i = FaceIndex; i >= 0; i--)
	{
		FVoxelFace& NextFace = FaceContainer[i + 1];

		// Elements are removed and it must be updated
		int BackTrackIndex = i;

		FVoxelFace* Face = &FaceContainer[BackTrackIndex];
		while (!FVoxelFace::MergeFaceUp(*Face, NextFace))
		{
			BackTrackIndex--;

			if (BackTrackIndex == -1 || GreedyMergeData.RowBorderCondition(*Face, NextFace))
			{
				ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NextFace, LocalVoxelTable, FaceDirectionIndex);
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
		ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, Face, LocalVoxelTable, FaceDirectionIndex);
	}

	FaceContainer.Empty();
}

void URunDirectionalVoxelMesher::ConvertFaceToProcMesh(TArray<FProcMeshSectionVars>& QuadMeshSectionArray,
                                                       const FVoxelFace& Face, TMap<uint32, uint32>& LocalVoxelTable,
                                                       const int FaceIndex) const
{
	const double VoxelSize = VoxelGenerator->GetVoxelSize();
	
	const auto VoxelId = Face.Voxel.VoxelId;
	int32 SectionId;
	if (!LocalVoxelTable.Contains(VoxelId))
	{
		SectionId = LocalVoxelTable.Num();
		// Keep track of how many voxel quads are actually displayed
		LocalVoxelTable.Add(VoxelId, SectionId);
	}
	else
	{
		SectionId = LocalVoxelTable[VoxelId];
	}

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

void URunDirectionalVoxelMesher::IncrementRun(const int X, const int Y, const int Z, const int32 AxisVoxelIndex,
                                              const bool bIsMinBorder, const bool bIsMaxBorder,
                                              const FMeshingDirections& FaceTemplate,
                                              const FMeshingDirections& ReversedFaceTemplate,
                                              const FMesherVariables& MeshVars, const UVoxelGrid& VoxelGridObject) const
{
	// Get voxel at current position of the run.
	const auto Position = FIntVector(X, Y, Z);

	if (!IsValid(VoxelGenerator) || !VoxelGridObject.VoxelGrid.IsValid())
	{
		return;
	}

	const int32 Index = VoxelGenerator->CalculateVoxelIndex(Position);
	const FVoxel Voxel = VoxelGridObject.VoxelGrid->GetData()[Index];

	// If voxel is empty, no mesh should be generated
	if (!Voxel.IsEmptyVoxel())
	{
		// Get correct face containers
		auto OriginalChunk = MeshVars.ChunkParams.OriginalChunk;
		const auto FaceContainerIndex = static_cast<uint8>(FaceTemplate.StaticMeshingData.FaceSide);
		const auto ReversedFaceContainerIndex = static_cast<uint8>(ReversedFaceTemplate.StaticMeshingData.FaceSide);

		// Generate face for each direction
		AddFace(VoxelGridObject, FaceTemplate, bIsMinBorder, Index, Position, Voxel, AxisVoxelIndex,
		        MeshVars.Faces[FaceContainerIndex], MeshVars.ChunkParams);
		AddFace(VoxelGridObject, ReversedFaceTemplate, bIsMaxBorder, Index, Position, Voxel, AxisVoxelIndex,
		        MeshVars.Faces[ReversedFaceContainerIndex], MeshVars.ChunkParams);
	}
}

void URunDirectionalVoxelMesher::AddFace(const UVoxelGrid& VoxelGridObject, const FMeshingDirections& FaceTemplate,
                                         const bool bIsBorder,
                                         const int32& Index, const FIntVector& Position, const FVoxel& Voxel,
                                         const int32& AxisVoxelIndex,
                                         const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces,
                                         const FChunkParams& ChunkParams)
{
	// Calculate indices need to check if face should be generated
	const FVoxelIndexParams VoxelIndexParams =
	{
		bIsBorder,
		FaceTemplate.ForwardVoxelIndex + Index,
		FaceTemplate.PreviousVoxelIndex + Index,
		Index - AxisVoxelIndex + FaceTemplate.ChunkBorderIndex,
		Voxel,
		FaceTemplate.StaticMeshingData.FaceSide
	};

	// Check if face should be generated
	if (IsBorderVoxelVisible(VoxelIndexParams, ChunkParams) || IsVoxelVisible(VoxelGridObject, VoxelIndexParams))
	{
		// Generate new face with coordinates
		const FVoxelFace NewFace = FaceTemplate.StaticMeshingData.FaceCreator(Voxel, Position, 1);

		if (!ChunkFaces->IsEmpty())
		{
			// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
			FVoxelFace& PrevFace = ChunkFaces->Last();

			if (FaceTemplate.StaticMeshingData.RunDirectionFaceMerge(PrevFace, NewFace))
			{
				// Return when new face was merged
				return;
			}
		}

		ChunkFaces->Push(NewFace);
	}
}

bool URunDirectionalVoxelMesher::IsBorderVoxelVisible(const FVoxelIndexParams& FaceData,
                                                      const FChunkParams& ChunkStruct)
{
	if (FaceData.IsBorder)
	{
		// Check voxel visibility in side chunk (crosschunk)
		const auto FaceContainerIndex = static_cast<uint8>(FaceData.FaceDirection);
		const auto SideChunk = ChunkStruct.SideChunks[FaceContainerIndex];
		if (SideChunk != nullptr && SideChunk->VoxelModel != nullptr)
		{
			const auto& NextVoxel = SideChunk->VoxelModel->GetVoxelAtIndex(FaceData.CurrentVoxelIndex);
			return NextVoxel.IsTransparent() && NextVoxel != FaceData.CurrentVoxel;
		}

		return SideChunk == nullptr && ChunkStruct.ShowBorders;
	}
	return false;
}

bool URunDirectionalVoxelMesher::IsVoxelVisible(const UVoxelGrid& VoxelGridObject, const FVoxelIndexParams& FaceData)
{
	if (!FaceData.IsBorder && VoxelGridObject.VoxelGrid->IsValidIndex(FaceData.ForwardVoxelIndex))
	{
		// Check if next voxel is visible based on calculated index
		const auto NextVoxel = VoxelGridObject.VoxelGrid->GetData()[FaceData.ForwardVoxelIndex];
		return NextVoxel.IsTransparent() && NextVoxel != FaceData.CurrentVoxel;
	}
	return false;
}

void URunDirectionalVoxelMesher::ChangeVoxelId(const UVoxelGrid& VoxelGridObject, TMap<int32, uint32>& VoxelTable,
                                               const FVoxelChange& VoxelChange) const
{
	const auto Index = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);
	const FVoxel VoxelId = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);

	// Check if chunk and position is valid.
	if (VoxelGridObject.VoxelGrid->IsValidIndex(Index))
	{
		// Default unknown voxels are empty
		if (VoxelId.IsEmptyVoxel())
		{
			const FVoxel RemovedVoxel = VoxelGridObject.VoxelGrid->GetData()[Index];
			VoxelGenerator->RemoveVoxelFromChunkTable(VoxelTable, RemovedVoxel);

			// Make previous voxel position empty.
			VoxelGridObject.VoxelGrid->GetData()[Index] = VoxelId;
		}
		else
		{
			// If voxel is known we get specific Id
			VoxelGenerator->ChangeKnownVoxelAtIndex(*VoxelGridObject.VoxelGrid, VoxelTable, Index, VoxelId);
		}
	}
}
