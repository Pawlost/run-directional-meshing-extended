#include "VoxelMesher/RunDirectionalVoxelMesher.h"
/*
void URunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelEdit>& VoxelChanges)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<UVoxelGrid>(MeshVars.ChunkParams.OriginalChunk->VoxelModel.Get());

	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	const auto& VoxelGrid = *VoxelGridPtr;

/*	if (VoxelChange != nullptr)
	{
		ChangeVoxelId(VoxelGrid, MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable, *VoxelChange);
	}

	PreallocateArrays(MeshVars);
	FaceGeneration(VoxelGrid, MeshVars);
}

void URunDirectionalVoxelMesher::IncrementBorderRun(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars,
	const FMeshingDirections& FaceTemplate,
	const FIntVector& Position, const int BorderIndex) const
{
	const int32 Index = VoxelGenerator->CalculateVoxelIndex(Position);
	const FVoxel Voxel = VoxelGridObject.VoxelGrid->GetData()[Index];
			
	// Check voxel visibility in side chunk (crosschunk)
	const auto FaceContainerIndex = static_cast<uint8>(FaceTemplate.StaticMeshingData.FaceDirection);

	const FVoxelParams VoxelParams =
	{
		Voxel,
		Position
	};
			
	const FBorderVoxelIndexParams BorderVoxelIndexParams =
	{
		BorderIndex,
		FaceTemplate.StaticMeshingData,
		VoxelParams
	};
			
	if (IsBorderVoxelVisible(BorderVoxelIndexParams, MeshVars.ChunkParams))
	{
	//	AddFace(FaceTemplate, VoxelParams, MeshVars.Faces[FaceContainerIndex]);
	}
}

void URunDirectionalVoxelMesher::CheckBorderX(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
/*	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(0, Y, Z);
	const auto ReversePosition = FIntVector(ChunkDimension - 1, Y, Z);
			
	// Back
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, BackFaceTemplate, Position, BorderIndex);

	// Front
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, FrontFaceTemplate, ReversePosition, BorderIndex);
}

void URunDirectionalVoxelMesher::CheckBorderY(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
/*	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(Y, 0, Z);
	const auto ReversePosition = FIntVector(Y, ChunkDimension - 1, Z);
			
	// Left
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, LeftFaceTemplate, Position, BorderIndex);

	// Right
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, RightFaceTemplate, ReversePosition, BorderIndex);
}

void URunDirectionalVoxelMesher::CheckBorderZ(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars, const int Y, const int Z) const
{
/*	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const auto Position = FIntVector(Z, Y, 0);
	const auto ReversePosition = FIntVector(Z, Y, ChunkDimension - 1);
			
	// Bottom
	int32 BorderIndex = VoxelGenerator->CalculateVoxelIndex(ReversePosition);
	IncrementBorderRun(VoxelGridObject, MeshVars, BottomFaceTemplate, Position, BorderIndex);

	// Top
	BorderIndex = VoxelGenerator->CalculateVoxelIndex(Position);
	IncrementBorderRun(VoxelGridObject, MeshVars, TopFaceTemplate, ReversePosition, BorderIndex);
}


void URunDirectionalVoxelMesher::FaceGeneration(const UVoxelGrid& VoxelGridObject, const FMesherVariables& MeshVars) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - RunDirectionalMeshing from VoxelGrid generation")
#endif

	const auto ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();

	if (!IsValid(VoxelGenerator))
	{
		return;
	}

	for (uint32 z = 0; z < ChunkDimension; z++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			CheckBorderX(VoxelGridObject, MeshVars, y, z);
			CheckBorderY(VoxelGridObject, MeshVars, y, z);
			CheckBorderZ(VoxelGridObject, MeshVars, y, z);
		}
	}
		
	// Local voxel table 
	TMap<uint32, uint32> LocalVoxelTable;
	SIZE_T GlobalVertexCount = 0;
	
	// Traverse through voxel grid
	for (uint32 x = 0; x < ChunkDimension; x++)
	{
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
				*//*
				IncrementRun(x, y, z, BackFaceTemplate, FrontFaceTemplate,
				             MeshVars, VoxelGridObject);
				IncrementRun(y, x, z, LeftFaceTemplate, RightFaceTemplate,
				             MeshVars, VoxelGridObject);
				IncrementRun(z, y, x,BottomFaceTemplate, TopFaceTemplate,
				             MeshVars, VoxelGridObject);
			}
		}


		// Directional Greedy Meshing
		/*
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
/*	for (uint8 FaceIndex = 0; FaceIndex < CHUNK_FACE_COUNT; FaceIndex++)
	{
		auto FaceContainer = MeshVars.Faces[FaceIndex];

		// Create quad foreach face
		for (auto Face : *FaceContainer)
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, Face, LocalVoxelTable, FaceIndex);
		}
	}

	//GenerateProcMesh(MeshVars, LocalVoxelTable);

	if (!MeshVars.ChunkParams.OriginalChunk.IsValid() || LocalVoxelTable.IsEmpty())
	{
		return;
	}

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	const FString MapName = GetWorld()->GetMapName();
	FVoxelMeshingProfilingLogger::LogGeneratedVertices(MapName, GlobalVertexCount);
#endif
}

void URunDirectionalVoxelMesher::IncrementRun(const int X, const int Y, const int Z,
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
		const auto FaceContainerIndex = static_cast<uint8>(FaceTemplate.StaticMeshingData.FaceDirection);
		const auto ReversedFaceContainerIndex = static_cast<uint8>(ReversedFaceTemplate.StaticMeshingData.FaceDirection);

		const FVoxelParams VoxelParams =
		{
			Voxel,
			Position
		};
		
		// Generate face for each direction
		/*CheckVoxelNeighborhood(VoxelGridObject, FaceTemplate, Index, VoxelParams,
			MeshVars.Faces[FaceContainerIndex]);
		CheckVoxelNeighborhood(VoxelGridObject, ReversedFaceTemplate, Index, VoxelParams,
		        MeshVars.Faces[ReversedFaceContainerIndex]);
	}
}

void URunDirectionalVoxelMesher::CheckVoxelNeighborhood(const UVoxelGrid& VoxelGridObject, const FMeshingDirections& FaceTemplate,
                                         const int32& Index, const FVoxelParams& VoxelParams,
                                         const TSharedPtr<TArray<FVirtualVoxelFace>>& ChunkFaces)
{
	// Calculate indices need to check if face should be generated
	const FInnerVoxelIndexParams VoxelIndexParams =
	{
		FaceTemplate.ForwardVoxelIndex + Index,
		FaceTemplate.PreviousVoxelIndex + Index,
		VoxelParams,
	};
	
	// Check if face should be generated
	if (IsVoxelVisible(VoxelGridObject, VoxelIndexParams))
	{
		//AddFace(FaceTemplate, VoxelParams, ChunkFaces);
	}
}

bool URunDirectionalVoxelMesher::IsBorderVoxelVisible(const FBorderVoxelIndexParams& FaceData,
                                                      const FChunkParams& ChunkStruct)
{
	// Check voxel visibility in side chunk (crosschunk)
	const auto FaceContainerIndex = static_cast<uint8>(FaceData.StaticData.FaceDirection);
	const auto SideChunk = ChunkStruct.SideChunks[FaceContainerIndex];
	if (SideChunk != nullptr && SideChunk->VoxelModel != nullptr)
	{
		const auto& NextVoxel = SideChunk->VoxelModel->GetVoxelAtIndex(FaceData.SideChunkVoxelIndex);
		return NextVoxel.IsTransparent() && NextVoxel != FaceData.VoxelParams.CurrentVoxel;
	}

	return SideChunk == nullptr && ChunkStruct.ShowBorders;
}

bool URunDirectionalVoxelMesher::IsVoxelVisible(const UVoxelGrid& VoxelGridObject, const FInnerVoxelIndexParams& FaceData)
{
	if (VoxelGridObject.VoxelGrid->IsValidIndex(FaceData.ForwardVoxelIndex))
	{
		// Check if next voxel is visible based on calculated index
		const auto NextVoxel = VoxelGridObject.VoxelGrid->GetData()[FaceData.ForwardVoxelIndex];
		return NextVoxel.IsTransparent() && NextVoxel != FaceData.VoxelParams.CurrentVoxel;
	}
	return false;
}

void URunDirectionalVoxelMesher::ChangeVoxelId(const UVoxelGrid& VoxelGridObject, TMap<int32, uint32>& VoxelTable,
                                               const FVoxelEdit& VoxelChange) const
{
	const auto Index = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);
	const FVoxel VoxelId = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);

	// Check if chunk and position is valid.
	if (VoxelGridObject.VoxelGrid->IsValidIndex(Index))
	{
		// Default unknown voxels are empty
		if (VoxelId.IsEmptyVoxel())
		{
			// Make previous voxel position empty.
			VoxelGridObject.VoxelGrid->GetData()[Index] = VoxelId;
		}
		else
		{
			// If voxel is known we get specific Id
			VoxelGenerator->ChangeKnownVoxelAtIndex(*VoxelGridObject.VoxelGrid, Index, VoxelId);
		}
	}
}*/