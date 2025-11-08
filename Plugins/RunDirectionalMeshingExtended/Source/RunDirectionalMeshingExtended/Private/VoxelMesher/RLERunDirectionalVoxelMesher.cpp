#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "Log/VoxelMeshingProfilingLogger.h"

#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/RLEVoxel.h"
#include "Voxel/Grid/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::GenerateMesh(FMesherVariables& MeshVars, TArray<FVoxelEdit>& VoxelChange)
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

	const uint32 VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();

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
		// TODO: remove check when rewrite is finished
		int VoxelCount = VoxelGridPtr->RLEVoxelGrid->Num();
		for (int i = 0; i < VoxelCount - 2; i++)
		{
			auto NextVoxel = (*VoxelGridPtr->RLEVoxelGrid)[i + 1];
			auto Voxel = (*VoxelGridPtr->RLEVoxelGrid)[i];
			check(NextVoxel.Voxel != Voxel.Voxel)
		}
		VoxelGridPtr->RLEVoxelGrid = IndexParams.VoxelGrid;
	}
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

	const uint32 ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	const uint32 MaxChunkVoxelSequence = VoxelGenerator->GetVoxelCountPerChunk();
	const uint32 VoxelLayer = VoxelGenerator->GetVoxelCountPerVoxelPlane();

	TSharedPtr<TArray<FRLEVoxel>> OldVoxelGrid = nullptr;
	
	// Set first run to trigger first condition in while loop
	if (!IndexParams.VoxelChanges->IsEmpty())
	{
		OldVoxelGrid = IndexParams.VoxelGrid;
		IndexParams.VoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		IndexParams.VoxelGrid->Reserve(OldVoxelGrid->Num() + IndexParams.VoxelChanges->Num());
		IndexParams.EditEnabled = true;

		auto VoxelChange = IndexParams.VoxelChanges->Pop();
		auto ChangedVoxel = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);
		auto EditVoxelRun = FRLEVoxel{1, ChangedVoxel};
		uint32 EditEventIndex = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);

		// TODO: do when inserting
		while (!IndexParams.VoxelChanges->IsEmpty() && (IndexParams.VoxelChanges->Top().VoxelPosition - FIntVector(0,1,0)) == VoxelChange.VoxelPosition &&
			VoxelChange.VoxelName == IndexParams.VoxelChanges->Top().VoxelName)
		{
			VoxelChange = IndexParams.VoxelChanges->Pop();
			EditVoxelRun.RunLenght++;
		}
		
		int CopyVoxelRunIndex = -1;

		uint32 DeleteFirstRUn = 0;
		// First voxel in a chunk
		if (EditEventIndex == 0)
		{
			IndexParams.VoxelGrid->Add(EditVoxelRun);
			DeleteFirstRUn = EditVoxelRun.RunLenght;
			EditEventIndex = MaxChunkVoxelSequence + 1;
		}
		
		CopyVoxelRunIndex++;
		auto& CopyVoxel = (*OldVoxelGrid)[CopyVoxelRunIndex];
		CopyVoxel.RunLenght -= DeleteFirstRUn;
		IndexParams.VoxelGrid->Add(CopyVoxel);

		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();
		EditEventArray->Push(EditVoxelRun);
		
		IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent] = { EditEventArray, EditEventIndex, 0};
		IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent] = {OldVoxelGrid, DeleteFirstRUn, CopyVoxelRunIndex};
	}
	
	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = {IndexParams.VoxelGrid, 0, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {IndexParams.VoxelGrid, VoxelLayer, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {IndexParams.VoxelGrid, ChunkDimension, 0};

	int IndexSequenceBetweenEvents = 0;
	auto InitialPosition = FIntVector(0, 0, 0);
	FRLEVoxel* PreviousVoxelRun = nullptr;

	auto NumberOfVoxelsInChunk = VoxelGenerator->GetVoxelCountPerChunk();

	// Traverse through voxel grid
	while (IndexParams.CurrentMeshingEventIndex < MaxChunkVoxelSequence)
	{
		const uint32 X = IndexParams.CurrentMeshingEventIndex / (VoxelLayer);
		const uint32 Z = ((IndexParams.CurrentMeshingEventIndex / ChunkDimension) % ChunkDimension);
		uint32 Y = IndexParams.CurrentMeshingEventIndex % ChunkDimension;

		// Check borders
		do
		{
			// Reset interval flag
			IndexParams.NextMeshingEventIndex = NumberOfVoxelsInChunk;

			// Edit Interval
			if (IndexParams.EditEnabled)
			{
				auto& CopyEvent = IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent];
				auto& EditEvent = IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent];
				auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];

				uint32 RemainingIndex = 0;
				FVoxel CurrentVoxel;

				if (EditEvent.LastEventIndex == IndexParams.CurrentMeshingEventIndex)
				{
					auto& CopyVoxel = CopyEvent.GetCurrentVoxel();
					if (EditEvent.LastEventIndex == LeadingEvent.GetEventIndex())
					{
						CopyEvent.VoxelRunIndex++;
						CurrentVoxel = CopyVoxel.Voxel;
						RemainingIndex = CopyVoxel.RunLenght;
					}
					else
					{
						RemainingIndex = LeadingEvent.GetEventIndex() - IndexParams.CurrentMeshingEventIndex;
						LeadingEvent.GetCurrentVoxel().RunLenght -= RemainingIndex;
						CurrentVoxel = CopyVoxel.Voxel;
					}

					auto& EditVoxel = EditEvent.GetCurrentVoxel();
					while (EditVoxel.RunLenght > RemainingIndex)
					{
						CopyEvent.VoxelRunIndex++;
						CurrentVoxel =	CopyVoxel.Voxel;
						RemainingIndex += CopyVoxel.RunLenght;
					}

					if (IndexParams.VoxelGrid->Last().Voxel == EditVoxel.Voxel)
					{
						IndexParams.VoxelGrid->Last().RunLenght += EditVoxel.RunLenght;
					}else
					{
						IndexParams.VoxelGrid->Add(EditVoxel);
					}

					RemainingIndex -= EditVoxel.RunLenght;

					if (IndexParams.VoxelGrid->Last().Voxel == CurrentVoxel)
					{
						IndexParams.VoxelGrid->Last().RunLenght += RemainingIndex;
					}
					else if (RemainingIndex != 0)
					{
						IndexParams.VoxelGrid->Add(FRLEVoxel{RemainingIndex, CurrentVoxel});
					}else if (OldVoxelGrid->IsValidIndex(CopyEvent.VoxelRunIndex + 1))
					{
						CopyEvent.VoxelRunIndex++;
						auto NextCopyVoxel = CopyEvent.GetCurrentVoxel();
						
						if (NextCopyVoxel.Voxel == IndexParams.VoxelGrid->Last().Voxel)
						{
							IndexParams.VoxelGrid->Last().RunLenght += NextCopyVoxel.RunLenght;
							RemainingIndex += NextCopyVoxel.RunLenght;
							CopyEvent.VoxelRunIndex++;
							if (OldVoxelGrid->IsValidIndex(CopyEvent.VoxelRunIndex + 1))
							{
								NextCopyVoxel = CopyEvent.GetCurrentVoxel();
								IndexParams.VoxelGrid->Add(NextCopyVoxel);
							}
						}else
						{
							IndexParams.VoxelGrid->Add(NextCopyVoxel);
						}
					}
					
					CopyEvent.LastEventIndex = IndexParams.CurrentMeshingEventIndex + RemainingIndex + EditVoxel.RunLenght;

					if (!IndexParams.VoxelChanges->IsEmpty())
					{
						auto VoxelChange = IndexParams.VoxelChanges->Pop();
						auto ChangedVoxel = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);
						auto EditVoxelRun = FRLEVoxel{1, ChangedVoxel};
						uint32 EditEventIndex = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);
					
						// TODO: do when inserting
						while (!IndexParams.VoxelChanges->IsEmpty() && (IndexParams.VoxelChanges->Top().VoxelPosition - FIntVector(0,1,0)) == VoxelChange.VoxelPosition &&
							VoxelChange.VoxelName == IndexParams.VoxelChanges->Top().VoxelName)
						{
							VoxelChange = IndexParams.VoxelChanges->Pop();
							EditVoxelRun.RunLenght++;
						}
					
						EditEvent.LastEventIndex = EditEventIndex;
						EditEvent.VoxelGridPtr->Empty();
						EditEvent.VoxelGridPtr->Push(EditVoxelRun);
					}else
					{
						EditEvent.LastEventIndex = NumberOfVoxelsInChunk + 1;
					}
				}else
				{
					if (CopyEvent.GetEventIndex() == IndexParams.CurrentMeshingEventIndex)
					{
						CopyEvent.VoxelRunIndex++;
						IndexParams.VoxelGrid->Add(CopyEvent.GetCurrentVoxel());
						CopyEvent.LastEventIndex = IndexParams.
							CurrentMeshingEventIndex;
					}
				}
				
				IndexParams.TryUpdateNextMeshingEvent(CopyEvent.GetEventIndex());
				IndexParams.TryUpdateNextMeshingEvent(EditEvent.LastEventIndex);
			}
			
			auto FollowingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval];
			
			if (AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::LeadingInterval))
			{
				const auto& LeadingMeshingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
				auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();
				
				// Left
				if ((PreviousVoxelRun->Voxel.IsEmptyVoxel() || PreviousVoxelRun->Voxel.IsTransparent() && !
					LeadingMeshingEventVoxel.IsVoxelEmpty()) && Y != 0)
				{
					InitialPosition = FIntVector(X, Y, Z);
					CreateSideFace(*MeshVars.VirtualFaces[Left], FStaticMergeData::LeftFaceData, InitialPosition,
								   LeadingMeshingEventVoxel, IndexSequenceBetweenEvents);
				}

				// Right
				if (!PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.Voxel.IsEmptyVoxel() ||
						LeadingMeshingEventVoxel.Voxel.IsTransparent()) && InitialPosition.Y +
					IndexSequenceBetweenEvents != ChunkDimension)
				{
					CreateSideFace(*MeshVars.VirtualFaces[Right], FStaticMergeData::RightFaceData, InitialPosition,
								   *PreviousVoxelRun, IndexSequenceBetweenEvents + 1);
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension

			AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::FollowingXInterval);
			AdvanceMeshingEventInterval(IndexParams, EMeshingEventIndex::FollowingZInterval);
			
			auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
			auto& FollowingXEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval];
			auto& FollowingZEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval];
			auto BorderSample = LeadingEvent.GetCurrentVoxel();
			
			// Left border
			if (Y == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, X, Z), EFaceDirection::Left, BorderSample, 1);
			}

			//Back border
			if (X == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, Z), EFaceDirection::Back, BorderSample,
								BorderSample.RunLenght);
			}

			// Front border
			if (X == ChunkDimension - 1)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, Z), EFaceDirection::Front, BorderSample,
								BorderSample.RunLenght);
			}

			// Top border
			if (Z == ChunkDimension - 1)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, X), EFaceDirection::Top, BorderSample,
								ChunkDimension - Y);
			}

			// Bottom border
			if (Z == 0)
			{
				AddBorderSample(IndexParams, FIntVector(0, Y, X), EFaceDirection::Bottom, BorderSample,
								ChunkDimension - Y);
			}

			// Sample interval
			// Move interval one step ahead if at the run end
			bool IsLeadingEmpty = LeadingEvent.GetCurrentVoxel().IsVoxelEmpty();
			bool IsFollowingXEmpty = FollowingXEvent.GetCurrentVoxel().IsVoxelEmpty();
			bool IsFollowingZEmpty = FollowingZEvent.GetCurrentVoxel().IsVoxelEmpty();

			bool IsLeadingTransparent = LeadingEvent.GetCurrentVoxel().IsTransparent();
			bool IsFollowingXTransparent = FollowingXEvent.GetCurrentVoxel().IsTransparent();
			bool IsFollowingZTransparent = FollowingZEvent.GetCurrentVoxel().IsTransparent();

			// Generate culled faces
			if (!IsLeadingEmpty || !IsFollowingXEmpty || !IsFollowingZEmpty)
			{
				const uint32 MaxYSequence = (ChunkDimension - Y) + IndexParams.CurrentMeshingEventIndex;
				IndexParams.TryUpdateNextMeshingEvent(MaxYSequence);

				IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;

				InitialPosition = FIntVector(X, Y, Z);

				CreateFace(MeshVars, FStaticMergeData::FrontFaceData, InitialPosition,
						   FollowingXEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
						   X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
							   IsFollowingXTransparent)));

				CreateFace(MeshVars, FStaticMergeData::TopFaceData, InitialPosition,
						   FollowingZEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
						   Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
							   IsFollowingZTransparent)));

				CreateFace(MeshVars, FStaticMergeData::BackFaceData, InitialPosition,
						   LeadingEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
						   X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
							   IsFollowingXTransparent)));

				CreateFace(MeshVars, FStaticMergeData::BottomFaceData, InitialPosition,
						   LeadingEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
						   Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
							   IsFollowingZTransparent)));
			}
			else
			{
				// Skip large empty sequences
				IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;
			}

			// Meshing event was finished
			IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
			Y += IndexSequenceBetweenEvents;

			PreviousVoxelRun = &IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
		}
		while (Y < ChunkDimension);

		// Right Border
		if (Y == ChunkDimension)
		{
			AddBorderSample(IndexParams, FIntVector(0, X, Z), EFaceDirection::Right, *PreviousVoxelRun, 1);
		}
	}

	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			DirectionalGreedyMerge(*MeshVars.ChunkMeshData, MeshVars.LocalVoxelTable,
								   FaceTemplates[f].StaticMeshingData, (*MeshVars.VirtualFaces[f])[y]);
		}
	}
}

void URLERunDirectionalVoxelMesher::BorderGeneration(FMesherVariables& MeshVars,
                                                     TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks) const
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Border generation - RLE RunDirectionalMeshing generation")
#endif

	const uint32 ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlane = VoxelGenerator->GetVoxelCountPerVoxelPlane();

	TArray<FVirtualVoxelFace> FaceContainer;
	FaceContainer.Reserve(VoxelPlane);

	TArray<FVirtualVoxelFace> InverseFaceContainer;
	FaceContainer.Reserve(VoxelPlane);

	// Generate border
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		const auto BorderChunk = BorderChunks[f];

		// TODO: rewrite ShowBorders
		if ((BorderChunk->IsSampled && BorderChunk->IsInverseSampled ||
				(MeshVars.ChunkParams.ShowBorders && (BorderChunk->IsSampled || BorderChunk->IsInverseSampled)))
			&& !BorderChunk->IsGenerated)
		{
			BorderChunk->IsGenerated = true;
			auto& FaceTemplate = FaceTemplates[f];
			auto InverseDirection = FaceTemplate.StaticMeshingData.InverseFaceDirection;
			auto& InverseFaceTemplate = FaceTemplates[InverseDirection];

			for (uint32 x = 0; x < ChunkDimension; x++)
			{
				for (uint32 y = 0; y < ChunkDimension; y++)
				{
					GenerateBorder(FaceContainer, InverseFaceContainer, BorderChunks, FaceTemplate, InverseFaceTemplate,
					               x, y);
				}
			}

			if (!FaceContainer.IsEmpty() || !InverseFaceContainer.IsEmpty())
			{
				// TODO: run with only empty samples
				DirectionalGreedyMerge(*MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable,
				                       FaceTemplate.StaticMeshingData, FaceContainer);

				DirectionalGreedyMerge(*MeshVars.BorderChunkMeshData, MeshVars.BorderLocalVoxelTable,
				                       InverseFaceTemplate.StaticMeshingData, InverseFaceContainer);
			}
		}
	}
}

bool URLERunDirectionalVoxelMesher::AdvanceMeshingEventInterval(FIndexParams& IndexParams,
                                                                const EMeshingEventIndex IntervalFlagIndex)
{
	bool AdvanceInterval = false;
	auto& Interval = IndexParams.MeshingEvents[IntervalFlagIndex];
	if (Interval.GetEventIndex() == IndexParams.CurrentMeshingEventIndex)
	{
		// Advance interval in chunk voxel sequence
		Interval.VoxelRunIndex++;
		Interval.LastEventIndex = IndexParams.CurrentMeshingEventIndex;
		AdvanceInterval = true;
	}

	IndexParams.TryUpdateNextMeshingEvent(Interval.GetEventIndex());

	return AdvanceInterval;
}

void URLERunDirectionalVoxelMesher::GenerateBorder(TArray<FVirtualVoxelFace>& FaceContainer,
                                                   TArray<FVirtualVoxelFace>& InverseFaceContainer,
                                                   TStaticArray<TSharedPtr<FBorderChunk>, CHUNK_FACE_COUNT>&
                                                   BorderChunks,
                                                   const FMeshingDirections& FaceTemplate,
                                                   const FMeshingDirections& InverseFaceTemplate, int X, int Y) const
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

	SmearVoxelBorder(SampledVoxel, *BorderChunk->BorderVoxelSamples, Index);
	SmearVoxelBorder(InverseSampledVoxel, *BorderChunk->InversedBorderVoxelSamples, Index);

	if (FaceTemplate.StaticMeshingData.IsInverseDirection)
	{
		Swap(SampledVoxel, InverseSampledVoxel);
	}

	if (!SampledVoxel.IsVoxelEmpty() && InverseSampledVoxel.IsVoxelEmpty() || !SampledVoxel.IsVoxelEmpty() && !
		SampledVoxel.IsTransparent() && InverseSampledVoxel.IsTransparent())
	{
		const FVirtualVoxelFace NewFace = FaceTemplate.StaticMeshingData.FaceCreator(
			SampledVoxel.Voxel, QuadPosition, 1);
		AddFace(FaceTemplate.StaticMeshingData, NewFace, FaceContainer);
	}

	if (!InverseSampledVoxel.IsVoxelEmpty() && SampledVoxel.IsVoxelEmpty() || !InverseSampledVoxel.IsVoxelEmpty() && !
		InverseSampledVoxel.IsTransparent() && SampledVoxel.IsTransparent())
	{
		const FVirtualVoxelFace NewFace = InverseFaceTemplate.StaticMeshingData.FaceCreator(
			InverseSampledVoxel.Voxel, QuadPosition, 1);
		AddFace(InverseFaceTemplate.StaticMeshingData, NewFace, InverseFaceContainer);
	}
}

void URLERunDirectionalVoxelMesher::SmearVoxelBorder(FRLEVoxel& CurrentVoxel, TArray<FRLEVoxel>& BorderVoxelSamples,
                                                     const int Index)
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
                                               const int YEnd, bool CanGenerate)
{
	if (CanGenerate)
	{
		const FVirtualVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
		auto MainDirectionIndex = StaticData.GetMainDirection(InitialPosition);
		AddFace(StaticData, NewFace, (*MeshVars.VirtualFaces[StaticData.FaceDirection])[MainDirectionIndex]);
	}
}

void URLERunDirectionalVoxelMesher::CreateSideFace(TArray<TArray<FVirtualVoxelFace>>& SideFaceData,
                                                   const FStaticMergeData& StaticData,
                                                   const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
                                                   const int YEnd)
{
	const FVirtualVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
	check(NewFace.Voxel.VoxelId != 0);

	TArray<FVirtualVoxelFace>& PrevFaces = SideFaceData[NewFace.StartVertexDown.Y];

	if (PrevFaces.IsEmpty() || !StaticData.RunDirectionFaceMerge(PrevFaces.Last(), NewFace))
	{
		PrevFaces.Push(NewFace);
	}
}

// TODO: optimize parameters
void URLERunDirectionalVoxelMesher::AddBorderSample(const FIndexParams& IndexParams, const FIntVector IndexCoords,
                                                    const EFaceDirection FaceDirection, const FRLEVoxel& VoxelSample,
                                                    const int RunLenght) const
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
