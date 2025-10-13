#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "Log/VoxelMeshingProfilingLogger.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelChange>& VoxelChange)
{

#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<URLEVoxelGrid>(MeshVars.ChunkParams.OriginalChunk->VoxelModel);
	
	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	FIndexParams IndexParams;
	IndexParams.VoxelChanges = &VoxelChange;
	
	const int VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();

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
			if (!BorderChunk->IsInverseSampled)
			{
				BorderChunk->IsInverseSampled = false;
				BorderChunk->InversedBorderVoxelSamples->Reset();
				BorderChunk->InversedBorderVoxelSamples->SetNum(VoxelLayer);
			}
		}else
		{
			auto BorderChunk = BorderChunks[d];
			BorderChunkSamples = BorderChunk->BorderVoxelSamples;
			IsSampled = &BorderChunk->IsSampled;
			if (!BorderChunk->IsSampled)
			{
				BorderChunk->IsSampled = false;
				BorderChunk->BorderVoxelSamples->Reset();
				BorderChunk->BorderVoxelSamples->SetNum(VoxelLayer);
			}
		}
		
		BorderChunks[d]->IsGenerated = false;
		IndexParams.SampledBorderChunks[d] = BorderChunkSamples;
		IsBorderSampled[d] = IsSampled;
	}
	
	IndexParams.VoxelGrid = VoxelGridPtr->RLEVoxelGrid;

	PreallocateArrays(MeshVars);

	FaceGeneration(IndexParams, MeshVars);

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

	if (IndexParams.EditEnabled)
	{
		 VoxelGridPtr->RLEVoxelGrid = IndexParams.VoxelGrid;
	}
}

URLERunDirectionalVoxelMesher::URLERunDirectionalVoxelMesher()
{
	// FullCulledFace
	MeshingData[EMeshingTypeIndex::FullCulledFaceInterval] = {};

	// FrontTopFace
	MeshingData[EMeshingTypeIndex::FrontTopFaceInterval] = {
		{FStaticMergeData::FrontFaceData, EMeshingEventIndex::FollowingXInterval},
		{FStaticMergeData::TopFaceData, EMeshingEventIndex::FollowingZInterval}
	};

	// BackFace
	MeshingData[EMeshingTypeIndex::BackFaceInterval] = {
		{FStaticMergeData::BackFaceData, EMeshingEventIndex::LeadingInterval}
	};

	// TopFace
	MeshingData[EMeshingTypeIndex::TopFaceInterval] = {
		{FStaticMergeData::TopFaceData, EMeshingEventIndex::FollowingZInterval}
	};

	// BottomFace
	MeshingData[EMeshingTypeIndex::BottomFaceInterval] = {
		{FStaticMergeData::BottomFaceData,  EMeshingEventIndex::LeadingInterval}
	};

	// FrontFace
	MeshingData[EMeshingTypeIndex::FrontFaceInterval] = {
		{FStaticMergeData::FrontFaceData,  EMeshingEventIndex::FollowingXInterval}
	};

	// BackBottomFace
	MeshingData[EMeshingTypeIndex::BackBottomFaceInterval] = {
		{FStaticMergeData::BackFaceData, EMeshingEventIndex::LeadingInterval},
		{FStaticMergeData::BottomFaceData, EMeshingEventIndex::LeadingInterval}
	};

	// EmptyFace
	MeshingData[EMeshingTypeIndex::EmptyFaceInterval] = {};
}

void URLERunDirectionalVoxelMesher::CompressVoxelGrid(FChunk& Chunk, TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	auto VoxelGridObject = NewObject<URLEVoxelGrid>();

	const auto RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

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

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, FMesherVariables& MeshVars)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif
	
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	const int MaxChunkVoxelSequence = VoxelGenerator->GetVoxelCountPerChunk();
	const int VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();
	
	auto& GridData = *IndexParams.VoxelGrid;
	auto FirstRun = &GridData[0];

	TSharedPtr<TArray<FRLEVoxel>> OldVoxelGrid = nullptr;
	// Set first run to trigger first condition in while loop
	
	if (!IndexParams.VoxelChanges->IsEmpty())
	{
		OldVoxelGrid = IndexParams.VoxelGrid;
		IndexParams.VoxelGrid =	MakeShared<TArray<FRLEVoxel>>();
		IndexParams.VoxelGrid->Reserve(IndexParams.VoxelGrid->Num() + 1);
		IndexParams.EditEnabled = true;
		
		auto VoxelChange = IndexParams.VoxelChanges->Pop();
		auto ChangedVoxel = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);
		auto EditVoxelRun = FRLEVoxel{1, ChangedVoxel};
		
		uint32 EventIndex = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);

		IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent] = {&EditVoxelRun, EventIndex, -1};
		IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent] = {nullptr, 0, -1};
	}
	
	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = {FirstRun, FirstRun->RunLenght, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {FirstRun,  FirstRun->RunLenght + VoxelLayer, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {FirstRun, FirstRun->RunLenght + ChunkDimension, 0 };
	
	int IndexSequenceBetweenEvents = 0;
	auto InitialPosition = FIntVector(0, 0, 0);
	const FRLEVoxel* PreviousVoxelRun = nullptr;

	auto VoxelChunk = VoxelGenerator->GetVoxelCountPerChunk();
	
	// Traverse through voxel grid
	while (IndexParams.CurrentMeshingEventIndex < MaxChunkVoxelSequence)
	{
		const int X = IndexParams.CurrentMeshingEventIndex / (VoxelLayer);
		const int Z = ((IndexParams.CurrentMeshingEventIndex / ChunkDimension) % ChunkDimension);
		int Y = IndexParams.CurrentMeshingEventIndex % ChunkDimension;
		
		// Check borders
		do
		{
			// Reset interval flag
			IndexParams.CurrentMeshingType = EMeshingTypeIndex::FullCulledFaceInterval;
			IndexParams.NextMeshingEventIndex = VoxelChunk;

			// Edit Interval
			if (IndexParams.EditEnabled)
			{
				auto& CopyEvent = IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent];
				auto& EditEvent = IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent];
				
				if (CopyEvent.EventIndex == IndexParams.CurrentMeshingEventIndex)
				{
					auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
					if (EditEvent.EventIndex == LeadingEvent.EventIndex)
					{
						auto& NextRLERun = (*OldVoxelGrid)[CopyEvent.VoxelRunIndex + 1];
						auto* LastRLERun = LeadingEvent.CurrentVoxelRun;
						NextRLERun.RunLenght--;
						int NextIndex = 1;
						if (LastRLERun->Voxel == EditEvent.CurrentVoxelRun->Voxel)
						{
							LeadingEvent.EventIndex += 1;
							LastRLERun->RunLenght++;
						}else
						{
							IndexParams.VoxelGrid->Add(*EditEvent.CurrentVoxelRun);
						}
						
						if (NextRLERun.IsRunEmpty())
						{
							CopyEvent.VoxelRunIndex++;
							if (OldVoxelGrid->IsValidIndex(CopyEvent.VoxelRunIndex + 1))
							{
								auto& NextCopyVoxel = (*OldVoxelGrid)[CopyEvent.VoxelRunIndex + 1];
								
								if (IndexParams.VoxelGrid->Last().Voxel == NextCopyVoxel.Voxel)
								{
									CopyEvent.VoxelRunIndex++;
									IndexParams.VoxelGrid->Last().RunLenght += NextCopyVoxel.RunLenght;
									NextIndex += NextCopyVoxel.RunLenght;

									if (&IndexParams.VoxelGrid->Last() == LastRLERun)
									{
										LeadingEvent.EventIndex += NextCopyVoxel.RunLenght;
									}
								}
							}
						}
				
						EditEvent.EventIndex = VoxelChunk + 1;
					
						CopyEvent.CurrentVoxelRun = &(*OldVoxelGrid)[CopyEvent.VoxelRunIndex];
						CopyEvent.EventIndex = NextIndex + IndexParams.CurrentMeshingEventIndex;
					}else
					{
						CopyEvent.VoxelRunIndex++;
						CopyEvent.CurrentVoxelRun = &(*OldVoxelGrid)[CopyEvent.VoxelRunIndex];
						IndexParams.VoxelGrid->Add(*CopyEvent.CurrentVoxelRun);
						CopyEvent.EventIndex = CopyEvent.CurrentVoxelRun->RunLenght + IndexParams.CurrentMeshingEventIndex;
					}
				}
					
				if (EditEvent.EventIndex == IndexParams.CurrentMeshingEventIndex)
				{
					IndexParams.ContinueEditIndex = 0;
					auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
					auto& FollowingXEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval];
					auto& FollowingZEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval];
					uint32 SubstractIndex = 0;

					if (LeadingEvent.CurrentVoxelRun->Voxel != EditEvent.CurrentVoxelRun->Voxel)
					{
						uint32 RemainingIndex = LeadingEvent.EventIndex - EditEvent.EventIndex;
						if (RemainingIndex == 1){
							if (OldVoxelGrid->IsValidIndex(CopyEvent.VoxelRunIndex + 1))
							{
								auto& NextVoxel = (*OldVoxelGrid)[CopyEvent.VoxelRunIndex + 1];
								if (NextVoxel.Voxel == EditEvent.CurrentVoxelRun->Voxel)
								{
									NextVoxel.RunLenght++;
									CopyEvent.VoxelRunIndex++;
									CopyEvent.CurrentVoxelRun = &NextVoxel;
									IndexParams.VoxelGrid->Add(NextVoxel);
									CopyEvent.EventIndex = CopyEvent.CurrentVoxelRun->RunLenght + IndexParams.CurrentMeshingEventIndex;
								}else
								{
									IndexParams.VoxelGrid->Add(*EditEvent.CurrentVoxelRun);
								}
								
								LeadingEvent.EventIndex--;
								(*IndexParams.VoxelGrid)[LeadingEvent.VoxelRunIndex].RunLenght -= 1;
								SubstractIndex = 1;
							}
						}
						else
						{
							uint32 OldRunLenght = LeadingEvent.CurrentVoxelRun->RunLenght;
							(*IndexParams.VoxelGrid)[LeadingEvent.VoxelRunIndex].RunLenght = OldRunLenght - RemainingIndex;
							IndexParams.VoxelGrid->Add(*EditEvent.CurrentVoxelRun);
							IndexParams.VoxelGrid->Add({RemainingIndex - 1, LeadingEvent.CurrentVoxelRun->Voxel});
							LeadingEvent.EventIndex = IndexParams.CurrentMeshingEventIndex;
							SubstractIndex = RemainingIndex;
						}
					} 

					if (LeadingEvent.CurrentVoxelRun == FollowingXEvent.CurrentVoxelRun)
					{
						FollowingXEvent.EventIndex -= SubstractIndex;	
					}

					if (LeadingEvent.CurrentVoxelRun == FollowingZEvent.CurrentVoxelRun)
					{
						FollowingZEvent.EventIndex -= SubstractIndex;
					}
					
					// TODO: rewrite
					if (!IndexParams.VoxelChanges->IsEmpty())
					{
					}
				
					EditEvent.EventIndex = VoxelChunk + 1;
				}

				IndexParams.NextMeshingEventIndex = CopyEvent.EventIndex;
				IndexParams.TryUpdateNextMeshingEvent(EditEvent.EventIndex);
			}
			
			if (AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::LeadingInterval, false, false))
			{
				const auto& LeadingMeshingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];

				// Generate Left and Right faces when interval advances
				// Left
				if (PreviousVoxelRun->Voxel.IsEmptyVoxel() && Y != 0)
				{
					InitialPosition = FIntVector(X, Y, Z);
					CreateSideFace(*MeshVars.VirtualFaces[Left], FStaticMergeData::LeftFaceData, InitialPosition, *LeadingMeshingEvent.CurrentVoxelRun, IndexSequenceBetweenEvents);
				}

				// Right
				if (LeadingMeshingEvent.CurrentVoxelRun->Voxel.IsEmptyVoxel() && InitialPosition.Y + IndexSequenceBetweenEvents != ChunkDimension)
				{
					CreateSideFace(*MeshVars.VirtualFaces[Right], FStaticMergeData::RightFaceData, InitialPosition, *PreviousVoxelRun, IndexSequenceBetweenEvents + 1);
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension
			auto& LeadingInterval = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
			auto BorderVoxel = *LeadingInterval.CurrentVoxelRun;
			
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

			AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::FollowingXInterval, X == 0, LeadingInterval.CurrentVoxelRun->Voxel.IsEmptyVoxel());
			AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::FollowingZInterval, Z == 0, LeadingInterval.CurrentVoxelRun->Voxel.IsEmptyVoxel());
			
			// Generate culled faces
			if (IndexParams.CurrentMeshingType != EMeshingTypeIndex::EmptyFaceInterval)
			{
				const uint32 MaxYSequence = (ChunkDimension - Y) + IndexParams.CurrentMeshingEventIndex;
				IndexParams.TryUpdateNextMeshingEvent(MaxYSequence);

				IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;

				InitialPosition = FIntVector(X, Y, Z);
				
				// Inner culling
				for (const auto& [FaceData, IntervalEndIndex] : MeshingData[IndexParams.CurrentMeshingType])
				{
					CreateFace(MeshVars, FaceData, InitialPosition,
							   *IndexParams.MeshingEvents[IntervalEndIndex].CurrentVoxelRun, IndexSequenceBetweenEvents);
				}
			}
			else
			{
				IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;
			}

			// Meshing event was finished
			IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
			Y += IndexSequenceBetweenEvents;

			PreviousVoxelRun = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].CurrentVoxelRun;

		} while (Y < ChunkDimension);

		// Right Border
		if(Y == ChunkDimension)
		{
			AddBorderSample(IndexParams, FIntVector(0, X, Z), EFaceDirection::Right, *PreviousVoxelRun, 1);
		}
	}

	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (int y = 0; y < ChunkDimension; y++)
		{
			DirectionalGreedyMerge(*MeshVars.ChunkMeshData, MeshVars.LocalVoxelTable, FaceTemplates[f].StaticMeshingData, (*MeshVars.VirtualFaces[f])[y]);
		}
	}
}

void URLERunDirectionalVoxelMesher::BorderGeneration(FMesherVariables& MeshVars, TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Border generation - RLE RunDirectionalMeshing generation")
#endif
	
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	const int VoxelPlane = VoxelGenerator->GetVoxelCountPerVoxelPlane();

	TArray<FVoxelFace> FaceContainer;
	FaceContainer.Reserve(VoxelPlane);

	TArray<FVoxelFace> InverseFaceContainer;
	FaceContainer.Reserve(VoxelPlane);
	
	// Generate border
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		const auto BorderChunk = BorderChunks[f];
		
		if ((BorderChunk->IsSampled && BorderChunk->IsInverseSampled ||
			(MeshVars.ChunkParams.ShowBorders && (BorderChunk->IsSampled || BorderChunk->IsInverseSampled)))
			&& !BorderChunk->IsGenerated)
		{
			BorderChunk->IsGenerated = true;
			auto& FaceTemplate = FaceTemplates[f];
			auto InverseDirection = FaceTemplate.StaticMeshingData.InverseFaceDirection;
			auto& InverseFaceTemplate = FaceTemplates[InverseDirection];
			
			for (int x = 0; x < ChunkDimension; x++)
			{
				for (int y = 0; y < ChunkDimension; y++)
				{
					GenerateBorder(FaceContainer, InverseFaceContainer, BorderChunks,  FaceTemplate, InverseFaceTemplate, x, y);
				}
			}

			if (!FaceContainer.IsEmpty() || !InverseFaceContainer.IsEmpty())
			{
				for (int x = 0; x < ChunkDimension; x++)
				{
					DirectionalGreedyMerge(*MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable,  FaceTemplate.StaticMeshingData, FaceContainer);
					DirectionalGreedyMerge(*MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable,  InverseFaceTemplate.StaticMeshingData, InverseFaceContainer);
				}
			}
		}
	}
}

bool URLERunDirectionalVoxelMesher::AdvanceMeshingEventInterval(FIndexParams& IndexParams, const EMeshingEventIndex IntervalFlagIndex,
	const bool BorderCondition, const bool LeadingIntervalEmpty)
{
	bool AdvanceInterval = false;
	auto& Interval = IndexParams.MeshingEvents[IntervalFlagIndex]; 
	if (Interval.EventIndex == IndexParams.CurrentMeshingEventIndex)
	{
		// Advance interval in chunk voxel sequence
		Interval.VoxelRunIndex++;
		Interval.CurrentVoxelRun = &(*IndexParams.VoxelGrid)[Interval.VoxelRunIndex];
		Interval.EventIndex = Interval.CurrentVoxelRun->RunLenght + IndexParams.CurrentMeshingEventIndex;
		AdvanceInterval = true;
	}
	
	IndexParams.TryUpdateNextMeshingEvent(Interval.EventIndex);

	// Sample interval
	// Move interval one step ahead if at the run end
	const bool IntervalEmptyVoxel = Interval.CurrentVoxelRun->IsVoxelEmpty();

	// Create mask based on interval combination, the mask is used as an index to array to prevent branching
	// Indexes are described in github documentation
	// Set interval flag
	// Using xor at the chunk border prevents generating quads facing inside
	IndexParams.CurrentMeshingType = static_cast<EMeshingTypeIndex>(IndexParams.CurrentMeshingType | ((BorderCondition && (LeadingIntervalEmpty != IntervalEmptyVoxel)) ^ IntervalEmptyVoxel) << IntervalFlagIndex);

	return AdvanceInterval;
}

void URLERunDirectionalVoxelMesher::GenerateBorder(TArray<FVoxelFace>& FaceContainer, TArray<FVoxelFace>& InverseFaceContainer,
	TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>& BorderChunks,
	const FMeshingDirections& FaceTemplate, const FMeshingDirections& InverseFaceTemplate, int X, int Y) const
{
	const int ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();

	
	FIntVector QuadPosition;
	FIntVector IndexPosition;
	
	FaceTemplate.StaticMeshingData.BorderLocation(IndexPosition, QuadPosition, X, Y, ChunkDimension);
	auto Index = VoxelGenerator->CalculateVoxelIndex(IndexPosition);
	
	// Smear border
	auto& FaceDirection = FaceTemplate.StaticMeshingData.FaceDirection;
	
	auto& BorderChunk = BorderChunks[FaceDirection];

	auto SampledVoxel = (*BorderChunk->BorderVoxelSamples)[Index];
	auto InverseSampledVoxel = (*BorderChunk->InversedBorderVoxelSamples)[Index];

	SmearVoxelBorder(SampledVoxel,*BorderChunk->BorderVoxelSamples, Index);
	SmearVoxelBorder(InverseSampledVoxel,*BorderChunk->InversedBorderVoxelSamples, Index);
	
	if(FaceTemplate.StaticMeshingData.IsInverseDirection){
		Swap(SampledVoxel, InverseSampledVoxel);
	}
	
	if (!SampledVoxel.IsVoxelEmpty() && InverseSampledVoxel.IsVoxelEmpty())
	{
		const FVoxelFace NewFace = FaceTemplate.StaticMeshingData.FaceCreator(SampledVoxel.Voxel, QuadPosition, 1);
		AddFace(FaceTemplate.StaticMeshingData, NewFace, FaceContainer);
	}

	if (!InverseSampledVoxel.IsVoxelEmpty() && SampledVoxel.IsVoxelEmpty())
	{
		const FVoxelFace NewFace = InverseFaceTemplate.StaticMeshingData.FaceCreator(InverseSampledVoxel.Voxel, QuadPosition, 1);
		AddFace(InverseFaceTemplate.StaticMeshingData, NewFace, InverseFaceContainer);
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

void URLERunDirectionalVoxelMesher::CreateFace(FMesherVariables& MeshVars,
                                               const FStaticMergeData& StaticData,
                                               const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
                                               const int YEnd)
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	auto MainDirectionIndex = StaticData.GetMainDirection(InitialPosition);
	AddFace(StaticData, NewFace, (*MeshVars.VirtualFaces[StaticData.FaceDirection])[MainDirectionIndex]);
}

void URLERunDirectionalVoxelMesher::CreateSideFace( TArray<TArray<FVoxelFace>>& SideFaceData, const FStaticMergeData& StaticData,
											   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
											   const int YEnd)
{
	const FVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	TArray<FVoxelFace>& PrevFaces = SideFaceData[NewFace.StartVertexDown.Y];

	if (PrevFaces.IsEmpty() || !StaticData.RunDirectionFaceMerge(PrevFaces.Last(), NewFace)) 
	{
		PrevFaces.Push(NewFace);
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