#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel, 
	TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces,
	TMap<int32, uint32> LocalVoxelTable,
	TMap<int32, uint32> BorderLocalVoxelTable,
	const TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData,
	TArray<FVoxelEdit>& VoxelChange,
	TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks,
	TSharedPtr<TArray<FRLEVoxel>>* SampledBorderChunks,
	bool ShowBorders)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	const auto VoxelGridPtr = Cast<URLEVoxelGrid>(VoxelModel.Get());

	if (VoxelGridPtr == nullptr)
	{
		return;
	}

	// TODO: make private class member, problematic writing because of parallel thread execution, maybe should be wrapped in a class
	FIndexParams IndexParams;
	IndexParams.VoxelChanges = &VoxelChange;
	IndexParams.SampledBorderChunks = SampledBorderChunks;
	
	IndexParams.VoxelGrid = VoxelGridPtr->RLEVoxelGrid;

	PreallocateArrays(VirtualFaces, ChunkMeshData);

	FaceGeneration(IndexParams, VirtualFaces);
	
	const uint32 ChunkDimension = VoxelGenerator->GetVoxelCountPerVoxelLine();
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			DirectionalGreedyMerge(*ChunkMeshData, LocalVoxelTable,
								   FaceTemplates[f].StaticMeshingData, (*VirtualFaces[f])[y]);
		}
	}
	
	BorderGeneration(BorderChunkMeshData, BorderLocalVoxelTable, BorderChunks, ShowBorders);
	
	if (IndexParams.EditEnabled)
	{
		// TODO: remove check when rewrite is finished
		VoxelGridPtr->RLEVoxelGrid = IndexParams.VoxelGrid;
		int VoxelCount = VoxelGridPtr->RLEVoxelGrid->Num();
		for (int i = 0; i < VoxelCount - 2; i++)
		{
			auto NextVoxel = (*VoxelGridPtr->RLEVoxelGrid)[i + 1];
			auto Voxel = (*VoxelGridPtr->RLEVoxelGrid)[i];
			check(NextVoxel.Voxel != Voxel.Voxel)
		}
	}
}

void URLERunDirectionalVoxelMesher::CompressVoxelModel(TStrongObjectPtr<UVoxelModel>& VoxelModel, TArray<FVoxel>& VoxelGrid)
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
	VoxelModel = TStrongObjectPtr<URLEVoxelGrid>(VoxelGridObject);
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams, const TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces)
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
		while (!IndexParams.VoxelChanges->IsEmpty() && (IndexParams.VoxelChanges->Top().VoxelPosition -
				FIntVector(0, 1, 0)) == VoxelChange.VoxelPosition &&
			VoxelChange.VoxelName == IndexParams.VoxelChanges->Top().VoxelName)
		{
			VoxelChange = IndexParams.VoxelChanges->Pop();
			EditVoxelRun.RunLenght++;
		}

		int CopyVoxelRunIndex = -1;

		uint32 RemainingIndex = 0;

		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();
		EditEventArray->Push(EditVoxelRun);
		IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent] = {EditEventArray, EditEventIndex, 0};

		CopyVoxelRunIndex++;
		auto CopyVoxel = (*OldVoxelGrid)[CopyVoxelRunIndex];
		uint32 Offset = 0;
		// First voxel in a chunk
		if (EditEventIndex == 0)
		{
			IndexParams.VoxelGrid->Add(EditVoxelRun);

			RemainingIndex = CopyVoxel.RunLenght;
			while (EditVoxelRun.RunLenght > RemainingIndex)
			{
				Offset += CopyVoxel.RunLenght;
				CopyVoxelRunIndex++;
				CopyVoxel = (*OldVoxelGrid)[CopyVoxelRunIndex];
				RemainingIndex += CopyVoxel.RunLenght;
			}
			
			AdvanceEditInterval(IndexParams);

			CopyVoxel.RunLenght = RemainingIndex - EditVoxelRun.RunLenght;

			if (IndexParams.VoxelGrid->Last().Voxel == CopyVoxel.Voxel)
			{
				IndexParams.VoxelGrid->Last().RunLenght += CopyVoxel.RunLenght;
			}else
			{
				IndexParams.VoxelGrid->Add(CopyVoxel);
			}
		}else
		{
			IndexParams.VoxelGrid->Add(CopyVoxel);
		}
		
		IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent] = {OldVoxelGrid, Offset, CopyVoxelRunIndex};
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
					auto CopyVoxel = CopyEvent.GetCurrentVoxel();
					if (EditEvent.LastEventIndex == LeadingEvent.GetEventIndex())
					{
						CopyEvent.AdvanceEvent();
						CopyVoxel = CopyEvent.GetCurrentVoxel();
						CurrentVoxel = CopyVoxel.Voxel;
						RemainingIndex = CopyVoxel.RunLenght;
					}
					else
					{
						RemainingIndex = LeadingEvent.GetEventIndex() - IndexParams.CurrentMeshingEventIndex;
						LeadingEvent.GetCurrentVoxel().RunLenght -= RemainingIndex;
						CurrentVoxel = CopyEvent.GetCurrentVoxel().Voxel;
					}

					auto& EditVoxel = EditEvent.GetCurrentVoxel();
					while (EditVoxel.RunLenght > RemainingIndex)
					{
						CopyEvent.AdvanceEvent();
						CopyVoxel = CopyEvent.GetCurrentVoxel();
						CurrentVoxel = CopyVoxel.Voxel;
						RemainingIndex += CopyVoxel.RunLenght;
					}

					if (IndexParams.VoxelGrid->Last().Voxel == EditVoxel.Voxel)
					{
						IndexParams.VoxelGrid->Last().RunLenght += EditVoxel.RunLenght;
					}
					else
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
					}

					if (OldVoxelGrid->IsValidIndex(CopyEvent.VoxelRunIndex + 1) && (*OldVoxelGrid)[CopyEvent.
						VoxelRunIndex + 1].Voxel == IndexParams.VoxelGrid->Last().Voxel)
					{
						CopyEvent.AdvanceEvent();
						IndexParams.VoxelGrid->Last().RunLenght += CopyEvent.GetCurrentVoxel().RunLenght;
					}

					AdvanceEditInterval(IndexParams);
				}
				else
				{
					int CopyEventIndex = CopyEvent.GetEventIndex();
					if (CopyEventIndex == IndexParams.CurrentMeshingEventIndex)
					{
						CopyEvent.AdvanceEvent();
						IndexParams.VoxelGrid->Add(CopyEvent.GetCurrentVoxel());
					}
				}

				IndexParams.TryUpdateNextMeshingEvent(CopyEvent.GetEventIndex());
				IndexParams.TryUpdateNextMeshingEvent(EditEvent.LastEventIndex);
			}

			if (AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval))
			{
				const auto& LeadingMeshingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
				auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();

				// Left
				if ((PreviousVoxelRun->Voxel.IsEmptyVoxel() || PreviousVoxelRun->Voxel.IsTransparent() && !
					LeadingMeshingEventVoxel.IsVoxelEmpty()) && Y != 0)
				{
					InitialPosition = FIntVector(X, Y, Z);
					CreateSideFace(*VirtualFaces[Left], FStaticMergeData::LeftFaceData, InitialPosition,
					               LeadingMeshingEventVoxel, IndexSequenceBetweenEvents);
				}

				// Right
				if (!PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.Voxel.IsEmptyVoxel() ||
						LeadingMeshingEventVoxel.Voxel.IsTransparent()) && InitialPosition.Y +
					IndexSequenceBetweenEvents != ChunkDimension)
				{
					CreateSideFace(*VirtualFaces[Right], FStaticMergeData::RightFaceData, InitialPosition,
					               *PreviousVoxelRun, IndexSequenceBetweenEvents + 1);
				}
			}

			// Calculate index
			// Smallest interval should always be increase of Y dimension

			AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingXInterval);
			AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingZInterval);

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

				CreateFace(VirtualFaces, FStaticMergeData::FrontFaceData, InitialPosition,
				           FollowingXEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
				           X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
					           IsFollowingXTransparent)));

				CreateFace(VirtualFaces, FStaticMergeData::TopFaceData, InitialPosition,
				           FollowingZEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
				           Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
					           IsFollowingZTransparent)));

				CreateFace(VirtualFaces, FStaticMergeData::BackFaceData, InitialPosition,
				           LeadingEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
				           X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
					           IsFollowingXTransparent)));

				CreateFace(VirtualFaces, FStaticMergeData::BottomFaceData, InitialPosition,
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
}

void URLERunDirectionalVoxelMesher::AdvanceEditInterval(FIndexParams& IndexParams) const
{
	auto& EditEvent = IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent];

	if (!IndexParams.VoxelChanges->IsEmpty())
	{
		auto VoxelChange = IndexParams.VoxelChanges->Pop();
		auto ChangedVoxel = VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName);
		auto EditVoxelRun = FRLEVoxel{1, ChangedVoxel};
		uint32 EditEventIndex = VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition);

		// TODO: do when inserting
		while (!IndexParams.VoxelChanges->IsEmpty() && (IndexParams.VoxelChanges->Top().VoxelPosition -
				FIntVector(0, 1, 0)) == VoxelChange.VoxelPosition &&
			VoxelChange.VoxelName == IndexParams.VoxelChanges->Top().VoxelName)
		{
			VoxelChange = IndexParams.VoxelChanges->Pop();
			EditVoxelRun.RunLenght++;
		}

		EditEvent.LastEventIndex = EditEventIndex;
		EditEvent.VoxelGridPtr->Empty();
		EditEvent.VoxelGridPtr->Push(EditVoxelRun);
	}
	else
	{
		EditEvent.LastEventIndex = VoxelGenerator->GetVoxelCountPerChunk() + 1;
	}
}

void URLERunDirectionalVoxelMesher::BorderGeneration(
	const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData, TMap<int32, uint32> BorderLocalVoxelTable,
	TStaticArray<TSharedPtr<FBorderChunk>, 6>& BorderChunks, bool ShowBorders)
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
				(ShowBorders && (BorderChunk->IsSampled || BorderChunk->IsInverseSampled)))
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
				DirectionalGreedyMerge(*BorderChunkMeshData, BorderLocalVoxelTable,
				                       FaceTemplate.StaticMeshingData, FaceContainer);

				DirectionalGreedyMerge(*BorderChunkMeshData, BorderLocalVoxelTable,
				                       InverseFaceTemplate.StaticMeshingData, InverseFaceContainer);
			}
		}
	}
}

bool URLERunDirectionalVoxelMesher::AdvanceMeshingEvent(FIndexParams& IndexParams,
                                                        const EMeshingEventIndex IntervalFlagIndex)
{
	bool AdvanceInterval = false;
	auto& Interval = IndexParams.MeshingEvents[IntervalFlagIndex];
	int IntervalEventIndex = Interval.GetEventIndex();
	if (IntervalEventIndex == IndexParams.CurrentMeshingEventIndex)
	{
		// Advance interval in chunk voxel sequence
		Interval.AdvanceEvent();
		AdvanceInterval = true;
	}

	IntervalEventIndex = Interval.GetEventIndex();
	IndexParams.TryUpdateNextMeshingEvent(IntervalEventIndex);

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

void URLERunDirectionalVoxelMesher::CreateFace(const TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>* VirtualFaces,
                                               const FStaticMergeData& StaticData,
                                               const FIntVector& InitialPosition, const FRLEVoxel& RLEVoxel,
                                               const int YEnd, const bool CanGenerate)
{
	if (CanGenerate)
	{
		const FVirtualVoxelFace NewFace = StaticData.FaceCreator(RLEVoxel.Voxel, InitialPosition, YEnd);
		const auto MainDirectionIndex = StaticData.GetMainDirection(InitialPosition);
		AddFace(StaticData, NewFace, (*VirtualFaces[StaticData.FaceDirection])[MainDirectionIndex]);
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
