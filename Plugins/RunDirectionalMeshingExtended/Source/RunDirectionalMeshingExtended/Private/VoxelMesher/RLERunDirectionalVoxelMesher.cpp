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
		
		if (FaceTemplate.StaticMeshingData.InverseFaceDirection)
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
			if (!BorderChunk->IsInverseSampled)
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
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX},
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BackFace
	IntervalFaces[EIntervalType::BackFaceInterval] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// TopFace
	IntervalFaces[EIntervalType::TopFaceInterval] = {
		{FStaticMergeData::TopFaceData, FIntVector(0, 0, -1), EIntervalEndIndex::FollowingZ}
	};

	// BottomFace
	IntervalFaces[EIntervalType::BottomFaceInterval] = {
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
	};

	// FrontFace
	IntervalFaces[EIntervalType::FrontFaceInterval] = {
		{FStaticMergeData::FrontFaceData, FIntVector(-1, 0, 0), EIntervalEndIndex::FollowingX}
	};

	// BackBottomFace
	IntervalFaces[EIntervalType::BackBottomFaceInterval] = {
		{FStaticMergeData::BackFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading},
		{FStaticMergeData::BottomFaceData, FIntVector(0, 0, 0), EIntervalEndIndex::Leading}
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
						CreateFace(MeshVars, LocalVoxelTable, FStaticMergeData::RightFaceData, InitialPosition, *PreviousRun, IntervalEnd, PreviousFaces);
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
				for (const auto& [FaceData, Offset, IntervalEndIndex] : IntervalFaces[IndexParams.IntervalFlag])
				{
					CreateFace(MeshVars, LocalVoxelTable, FaceData, InitialPosition + Offset,
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

		// Right
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

void URLERunDirectionalVoxelMesher::BorderGeneration(FMesherVariables& MeshVars, TMap<uint32, uint32>& LocalVoxelTable, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks) const
{
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerChunkDimension();
	// Generate border

	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		const auto BorderChunk = BorderChunks[f];
		
		if (BorderChunk->IsSampled && BorderChunk->IsInverseSampled && !BorderChunk->IsGenerated)
		{
			BorderChunk->IsGenerated = true;
			for (int x = 0; x < ChunkDimension; x++)
			{
				for (int y = 0; y < ChunkDimension; y++)
				{
					GenerateBorder(MeshVars, BorderChunks, LocalVoxelTable, FaceTemplates[f], x, y);
				}
			}
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

void URLERunDirectionalVoxelMesher::GenerateBorder(const FMesherVariables& MeshVars, TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks, TMap<uint32, uint32>& LocalVoxelTable, const FMeshingDirections& FaceTemplate, int X, int Y) const
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
	
	auto InverseFaceTemplate = FaceTemplates[FaceTemplate.StaticMeshingData.InverseFaceDirection];
	if (!SampledVoxel.IsVoxelEmpty())
	{
		const FVoxelFace NewFace = InverseFaceTemplate.StaticMeshingData.FaceCreator(SampledVoxel.Voxel, QuadPosition, 1);
		ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, FaceDirection);
	}

	if (!InverseSampledVoxel.IsVoxelEmpty())
	{
		const FVoxelFace NewFace = FaceTemplate.StaticMeshingData.FaceCreator(InverseSampledVoxel.Voxel, QuadPosition, 1);
		ConvertFaceToProcMesh(*MeshVars.QuadMeshSectionArray, NewFace, LocalVoxelTable, InverseFaceTemplate.StaticMeshingData.FaceDirection);
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