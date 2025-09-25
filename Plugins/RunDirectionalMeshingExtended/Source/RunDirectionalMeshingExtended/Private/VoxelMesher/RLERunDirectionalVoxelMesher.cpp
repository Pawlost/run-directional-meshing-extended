#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"

#include "Log/VoxelMeshingProfilingLogger.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, FVoxelChange* VoxelChange)
{
	if (EmptyActor(MeshVars))
	{
		return;
	}

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<URLEVoxelGrid>(MeshVars.ChunkParams.OriginalChunk->VoxelModel);
	
	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	FIndexParams IndexParams;
	
	const int VoxelLayer = VoxelGenerator->GetVoxelCountPerChunkLayer();

	bool* IsBorderSampled[CHUNK_FACE_COUNT];
	
	// Allocate Borders, after creating change
	for (int d = 0; d < CHUNK_FACE_COUNT; d++)
	{
		const auto& FaceTemplate = FaceTemplates[d];

		if (VoxelGridPtr->BorderChunks[d] == nullptr)
		{
			auto SideChunk = MeshVars.ChunkParams.SideChunks[FaceTemplate.StaticMeshingData.FaceDirection];
			if (SideChunk != nullptr && SideChunk->VoxelModel != nullptr)
			{
				auto SideVoxelGrid = Cast<URLEVoxelGrid>(SideChunk->VoxelModel);
				auto& BorderChunkPtr = SideVoxelGrid->BorderChunks[FaceTemplate.StaticMeshingData.InverseFaceDirection];

				if (BorderChunkPtr == nullptr)
				{
					BorderChunkPtr = MakeShared<FBorderChunk>(VoxelLayer);
				}

				VoxelGridPtr->BorderChunks[d] = BorderChunkPtr;
			}
			else
			{
				VoxelGridPtr->BorderChunks[d] = MakeShared<FBorderChunk>(VoxelLayer);
			}
		}

		TSharedPtr<TArray<FRLEVoxel>> BorderChunkSamples = nullptr;
		bool* IsSampled = nullptr;
		
		if (FaceTemplate.StaticMeshingData.IsInverseDirection)
		{
			auto BorderChunk = VoxelGridPtr->BorderChunks[d];
			if (!BorderChunk->IsInverseSampled)
			{
				BorderChunkSamples = BorderChunk->InversedBorderVoxelSamples;
				IsSampled = &VoxelGridPtr->BorderChunks[d]->IsInverseSampled;
			}
		}else
		{
			auto BorderChunk = VoxelGridPtr->BorderChunks[d];
			if (!BorderChunk->IsSampled)
			{
				BorderChunkSamples = VoxelGridPtr->BorderChunks[d]->BorderVoxelSamples;
				IsSampled = &VoxelGridPtr->BorderChunks[d]->IsSampled;
			}
		}
		
		VoxelGridPtr->BorderChunks[d]->IsGenerated = false;
		IndexParams.SampledBorderChunks[d] = BorderChunkSamples;
		IsBorderSampled[d] = IsSampled;
	}
	
	IndexParams.VoxelGrid = VoxelGridPtr->RLEVoxelGrid;

	if (VoxelChange != nullptr)
	{
		IndexParams.VoxelChange = VoxelChange;
		IndexParams.NewVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		IndexParams.NewVoxelGrid->Reserve(IndexParams.VoxelGrid->Num() + 1);
		IndexParams.EditVoxel = VoxelGenerator->GetVoxelByName(VoxelChange->VoxelName);

		if (!IndexParams.EditVoxel.IsEmptyVoxel())
		{
			auto& VoxelTable = MeshVars.ChunkParams.OriginalChunk->ChunkVoxelIdTable;
			if (!VoxelTable.Contains(IndexParams.EditVoxel.VoxelId))
			{
				VoxelTable.Add(IndexParams.EditVoxel.VoxelId, 0);
			}
		}
	}
	else
	{
		IndexParams.NewVoxelGrid = nullptr;
	}

	PreallocateArrays(MeshVars);
	
	TMap<uint32, uint32> LocalVoxelTable;

	FaceGeneration(IndexParams, MeshVars, LocalVoxelTable);

	for (int i = 0; i < CHUNK_FACE_COUNT; ++i)
	{
		auto IsSampled = IsBorderSampled[i];
		if (IsSampled != nullptr)
		{
			*(IsSampled) = true;
		}
	}
	
	BorderGeneration(MeshVars, LocalVoxelTable, VoxelGridPtr->BorderChunks);

	GenerateProcMesh(MeshVars, LocalVoxelTable);

	if (IndexParams.VoxelChange != nullptr)
	{
		VoxelGridPtr->RLEVoxelGrid = IndexParams.NewVoxelGrid;
	}
}

void URLERunDirectionalVoxelMesher::CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	auto VoxelGridObject = NewObject<URLEVoxelGrid>();

	const auto RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

	RLEVoxelGrid->Reserve(Chunk.ChunkVoxelIdTable.Num());
	RLEVoxelGrid->Emplace(1, VoxelGrid[0]);

	for (int32 x = 1; x < VoxelGrid.Num(); x++)
	{
		const FVoxel Voxel = VoxelGrid[x];
		if (RLEVoxelGrid->Last().Voxel == Voxel)
		{
			RLEVoxelGrid->Last().RunLenght++;
		}
		else
		{
			RLEVoxelGrid->Emplace(1, Voxel);
		}
	}

	VoxelGridObject->RLEVoxelGrid = RLEVoxelGrid;
	Chunk.VoxelModel = VoxelGridObject;

#if defined(UE_BUILD_DEBUG) || defined(UE_BUILD_DEVELOPMENT)
	const FString MapName = GetWorld()->GetMapName();
	FVoxelMeshingProfilingLogger::LogAllocatedMemory(MapName, VoxelGridObject->RLEVoxelGrid->GetAllocatedSize());
#endif
}

URLERunDirectionalVoxelMesher::URLERunDirectionalVoxelMesher()
{
	// FullCulledFace
	IntervalFaces[EIntervalType::FullCulledFaceInterval] = {};

	// FrontTopFace
	IntervalFaces[EIntervalType::FrontTopFaceInterval] = {
		{FStaticMergeData::FrontFaceData, EIntervalEndIndex::FollowingX},
		{FStaticMergeData::TopFaceData, EIntervalEndIndex::FollowingZ}
	};

	// BackFace
	IntervalFaces[EIntervalType::BackFaceInterval] = {
		{FStaticMergeData::BackFaceData, EIntervalEndIndex::Leading}
	};

	// TopFace
	IntervalFaces[EIntervalType::TopFaceInterval] = {
		{FStaticMergeData::TopFaceData, EIntervalEndIndex::FollowingZ}
	};

	// BottomFace
	IntervalFaces[EIntervalType::BottomFaceInterval] = {
		{FStaticMergeData::BottomFaceData,  EIntervalEndIndex::Leading}
	};

	// FrontFace
	IntervalFaces[EIntervalType::FrontFaceInterval] = {
		{FStaticMergeData::FrontFaceData,  EIntervalEndIndex::FollowingX}
	};

	// BackBottomFace
	IntervalFaces[EIntervalType::BackBottomFaceInterval] = {
		{FStaticMergeData::BackFaceData, EIntervalEndIndex::Leading},
		{FStaticMergeData::BottomFaceData, EIntervalEndIndex::Leading}
	};

	// EmptyFace
	IntervalFaces[EIntervalType::EmptyFaceInterval] = {};
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars,
                                                   TMap<uint32, uint32>& LocalVoxelTable)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif
	
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const int MaxChunkVoxelSequence = VoxelGenerator->GetVoxelCountPerChunk();
	const int VoxelLayer = VoxelGenerator->GetVoxelCountPerChunkLayer();

	const auto& GridData = *IndexParams.VoxelGrid;
	const auto FirstRun = &GridData[0];
	// Set first run to trigger first condition in while loop
	
	IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading] = {FirstRun, FirstRun->RunLenght, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingX] = {FirstRun,  FirstRun->RunLenght + VoxelLayer, 0};
	IndexParams.NextIntervalEnds[EIntervalEndIndex::FollowingZ] = {FirstRun, FirstRun->RunLenght + ChunkDimension, 0 };
	
	int IntervalEnd = 0;
	FVoxelFace PreviousFaces[CHUNK_FACE_COUNT];

	auto InitialPosition = FIntVector(0, 0, 0);

	const FRLEVoxel* PreviousRun = nullptr;
	
	// Traverse through voxel grid
	while (IndexParams.TraversedVoxelSequence < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.TraversedVoxelSequence / (VoxelLayer);
		const int Z = ((IndexParams.TraversedVoxelSequence / ChunkDimension) % ChunkDimension);
		int Y = IndexParams.TraversedVoxelSequence % ChunkDimension;
		
		// Check borders
		do
		{
			// Reset interval flag
			IndexParams.IntervalFlag = 0;
			
			if (AdvanceInterval(IndexParams, EIntervalEndIndex::Leading, false, false))
			{
				const auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];

				// Generate Left and Right faces when interval advances
				// Left
				if (PreviousRun->Voxel.IsEmptyVoxel() && Y != 0)
				{
					InitialPosition = FIntVector(X, Y, Z);
					CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::LeftFaceData, InitialPosition, *LeadingInterval.CurrentRun, IntervalEnd, PreviousFaces);
				}
				else if (LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel())
				{
					// Right
					if(InitialPosition.Y + IntervalEnd != ChunkDimension)
					{
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition, *PreviousRun, IntervalEnd + 1, PreviousFaces);
					}
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension
			auto& LeadingInterval = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading];

			auto BorderVoxel = *LeadingInterval.CurrentRun;

			// Left border
			if (Y == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, X, Z), EFaceDirection::Left, BorderVoxel, 1);
			}

			//Back border
			if (X == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, Z), EFaceDirection::Back, BorderVoxel, BorderVoxel.RunLenght);
			}

			// Front border
			if (X == ChunkDimension - 1)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, Z), EFaceDirection::Front, BorderVoxel, BorderVoxel.RunLenght);
			}

			// Top border
			if (Z == ChunkDimension - 1)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, X), EFaceDirection::Top, BorderVoxel,  ChunkDimension - Y);
			}

			// Bottom border
			if (Z == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, X), EFaceDirection::Bottom, BorderVoxel, ChunkDimension - Y);
			}
			
			IndexParams.MinValue = LeadingInterval.RunEnd;

			AdvanceInterval(IndexParams, EIntervalEndIndex::FollowingX, X == 0, LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel());
			AdvanceInterval(IndexParams, EIntervalEndIndex::FollowingZ, Z == 0, LeadingInterval.CurrentRun->Voxel.IsEmptyVoxel());
			
			// Generate culled faces
			if (IndexParams.IntervalFlag != EIntervalType::EmptyFaceInterval)
			{
				IndexParams.MinValue = FMath::Min(IndexParams.MinValue,(ChunkDimension - Y) + IndexParams.TraversedVoxelSequence);

				IntervalEnd = IndexParams.MinValue - IndexParams.TraversedVoxelSequence;

				InitialPosition = FIntVector(X, Y, Z);
				
				// Inner culling
				for (const auto& [FaceData, IntervalEndIndex] : IntervalFaces[IndexParams.IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition,
							   *IndexParams.NextIntervalEnds[IntervalEndIndex].CurrentRun, IntervalEnd, PreviousFaces);
				}
			}
			else
			{
				IntervalEnd = IndexParams.MinValue - IndexParams.TraversedVoxelSequence;
			}
			
			IndexParams.TraversedVoxelSequence = IndexParams.MinValue;
			Y += IntervalEnd;

			PreviousRun = IndexParams.NextIntervalEnds[EIntervalEndIndex::Leading].CurrentRun;

		} while (Y < ChunkDimension);

		// Right Border
		if(Y == ChunkDimension)
		{
			AddBorderSample(IndexParams, FIntVector(0, X, Z), EFaceDirection::Right, *PreviousRun, 1);
		}
	}

	// TODO: rewrite
	// TODO: move
	for (int direction = 0; direction < CHUNK_FACE_COUNT; direction++)
	{
		auto& PreviousFace = PreviousFaces[direction];
		if (!PreviousFace.Voxel.IsEmptyVoxel())
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, PreviousFace, LocalVoxelTable, direction);
		}
	}
}

void URLERunDirectionalVoxelMesher::BorderGeneration(FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks)
{
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	const int ChunkLayer = VoxelGenerator->GetVoxelCountPerChunkLayer();

	// Generate border
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		const auto BorderChunk = BorderChunks[f];
		
		if (BorderChunk->IsSampled && BorderChunk->IsInverseSampled && !BorderChunk->IsGenerated)
		{
			BorderChunk->IsGenerated = true;
			auto FaceTemplate = FaceTemplates[f];

			auto& ChunkFaces = MeshVars.Faces[FaceTemplate.StaticMeshingData.FaceDirection];
			ChunkFaces = MakeShared<TArray<FVoxelFace>>();
			
			auto& InverseChunkFaces = MeshVars.Faces[FaceTemplate.StaticMeshingData.InverseFaceDirection];
			InverseChunkFaces->Reserve(ChunkLayer);
			
			for (int x = 0; x < ChunkDimension; x++)
			{
				for (int y = 0; y < ChunkDimension; y++)
				{
					GenerateBorder(BorderChunks, ChunkFaces, InverseChunkFaces, FaceTemplate, x, y);
				}
			}

			DirectionalGreedyMerge(MeshVars, LocalVoxelTable, FaceTemplate.StaticMeshingData);
			DirectionalGreedyMerge(MeshVars, LocalVoxelTable, FaceTemplates[FaceTemplate.StaticMeshingData.InverseFaceDirection].StaticMeshingData);
		}
	}
}

bool URLERunDirectionalVoxelMesher::AdvanceInterval(FIndexParams& IndexParams, const EIntervalEndIndex IntervalFlagIndex,
	const bool BorderCondition, const bool LeadingIntervalEmpty)
{
	bool AdvanceInterval = false;
	if (auto& Interval = IndexParams.NextIntervalEnds[IntervalFlagIndex]; Interval.RunEnd == IndexParams.TraversedVoxelSequence)
	{
		// Advance interval in chunk voxel sequence
		Interval.RunIndex++;
		Interval.CurrentRun = &(*IndexParams.VoxelGrid)[Interval.RunIndex];
		Interval.RunEnd = Interval.CurrentRun->RunLenght + IndexParams.TraversedVoxelSequence;
		AdvanceInterval = true;
	}

	// Sample interval
	
	// Move interval one step ahead if at the run end
	const auto& Interval = IndexParams.NextIntervalEnds[IntervalFlagIndex];
	const bool IntervalEmptyVoxel = Interval.CurrentRun->IsVoxelEmpty();

	// Create mask based on interval combination, the mask is used as an index to array to prevent branching
	// Indexes are described in github documentation
	// Set interval flag
	// Using xor at the chunk border prevents generating quads facing inside
	IndexParams.IntervalFlag |= ((BorderCondition && (LeadingIntervalEmpty != IntervalEmptyVoxel)) ^ IntervalEmptyVoxel) << IntervalFlagIndex;
	IndexParams.MinValue = FMath::Min(Interval.RunEnd, IndexParams.MinValue);

	return AdvanceInterval;
}

void URLERunDirectionalVoxelMesher::GenerateBorder(
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks, const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces,
	const TSharedPtr<TArray<FVoxelFace>>& ReversedChunkFaces,
	const FMeshingDirections& FaceTemplate, int X, int Y)
{
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();

	auto InitialPosition = FIntVector(0, Y, X);
	auto Index = VoxelGenerator->CalculateVoxelIndex(InitialPosition);
	
	// Smear border
	auto& FaceDirection = FaceTemplate.StaticMeshingData.FaceDirection;

	auto& BorderChunk = BorderChunks[FaceDirection];

	auto& SampledVoxel = (*BorderChunk->BorderVoxelSamples)[Index];
	auto& InverseSampledVoxel = (*BorderChunk->InversedBorderVoxelSamples)[Index];

	SmearVoxelBorder(SampledVoxel,*BorderChunk->BorderVoxelSamples, Index);
	SmearVoxelBorder(InverseSampledVoxel,*BorderChunk->InversedBorderVoxelSamples, Index);
	
	// GenerateQuad
	const FIntVector QuadPosition =  FaceTemplate.StaticMeshingData.BorderLocation(X, Y, ChunkDimension);
	
	if(FaceTemplate.StaticMeshingData.IsInverseDirection){
		const auto Temp = SampledVoxel;
		SampledVoxel = InverseSampledVoxel;
		InverseSampledVoxel = Temp;
	}
	
	if (!SampledVoxel.IsVoxelEmpty() && InverseSampledVoxel.IsVoxelEmpty())
	{
		FVoxelParams FaceParam = {SampledVoxel.Voxel, QuadPosition};
		AddFace(FaceTemplate, FaceParam, ChunkFaces);
	}

	auto InverseFaceTemplate = FaceTemplates[FaceTemplate.StaticMeshingData.InverseFaceDirection];
	if (!InverseSampledVoxel.IsVoxelEmpty() && SampledVoxel.IsVoxelEmpty())
	{
		FVoxelParams FaceParam = {InverseSampledVoxel.Voxel, QuadPosition};
		AddFace(InverseFaceTemplate, FaceParam, ReversedChunkFaces);
	}
}

void URLERunDirectionalVoxelMesher::SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples, const int Index)
{
	if (CurrentVoxel.RunLenght > 1)
	{
		CurrentVoxel.RunLenght--;
		if (BorderVoxelSamples.IsValidIndex(Index + 1))
		{
			if (auto& NextIndex = BorderVoxelSamples[Index + 1]; NextIndex.RunLenght == 0)
			{
				NextIndex = CurrentVoxel;
			}
		}
	}
}

void URLERunDirectionalVoxelMesher::CreateFace(const FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable,
                                               const FStaticMergeData& StaticData,
                                               const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
                                               const int YEnd, FVoxelFace* PreviousFaces) const
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);

	auto& PreviousFace =  PreviousFaces[StaticData.FaceDirection];
	if (!PreviousFace.Voxel.IsEmptyVoxel())
	{
		// Tries to merge face coordinates into previous face. Because faces are sorted, the last one is always the correct one.
		if (!StaticData.RunDirectionFaceMerge(PreviousFace, NewFace))
		{
			ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, PreviousFace, LocalVoxelTable, StaticData.FaceDirection);
			PreviousFace = NewFace;
		}
	}else
	{
		PreviousFace = NewFace;
	}
}

// TODO: optimize parameters
void URLERunDirectionalVoxelMesher::AddBorderSample(const FIndexParams& IndexParams, const FIntVector IndexCoords, const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample, const int RunLenght) const
{
	const auto BorderSample = IndexParams.SampledBorderChunks[FaceDirection];
	if (BorderSample != nullptr)
	{
		const auto Index = VoxelGenerator->CalculateVoxelIndex(IndexCoords);
		auto& BorderVoxel = (*BorderSample)[Index];
		BorderVoxel = VoxelSample;
		BorderVoxel.RunLenght = RunLenght;
	}
}

void URLERunDirectionalVoxelMesher::AddFace(const FMeshingDirections& FaceTemplate, const FVoxelParams& FaceParams, const TSharedPtr<TArray<FVoxelFace>>& ChunkFaces)
{
	// Generate new face with coordinates
	const FVoxelFace NewFace = FaceTemplate.StaticMeshingData.FaceCreator(FaceParams.CurrentVoxel, FaceParams.FacePosition, 1);

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