#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"

// persistent preallocation must be maintained
void URLERunDirectionalVoxelMesher::GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
	TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
	TMap<int32, uint32>& LocalVoxelTable,
	TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
	TArray<FRLEVoxelEdit>& VoxelChanges,
	FBorderSamples& BorderSamples)
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
	IndexParams.SampledBorderChunks = &BorderSamples;
	IndexParams.VoxelEdits = &VoxelChanges;
	
	IndexParams.VoxelGrid = VoxelGridPtr->RLEVoxelGrid;
	IndexParams.VirtualFaces = VirtualFaces;
	
	FaceGeneration(IndexParams);
	
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			DirectionalGreedyMerge(*ChunkMeshData, LocalVoxelTable,
								   FaceTemplates[f].StaticMeshingData, (*VirtualFaces[f])[y]);
		}
	}
	
	if (IndexParams.EditEnabled)
	{
		VoxelGridPtr->RLEVoxelGrid = IndexParams.VoxelGrid;
	}
}

TStrongObjectPtr<UVoxelModel> URLERunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
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
	return  TStrongObjectPtr<URLEVoxelGrid>(VoxelGridObject);
}

void URLERunDirectionalVoxelMesher::FaceGeneration(FIndexParams& IndexParams)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif

	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();
	const uint32 VoxelLayer = VoxelData->GetVoxelCountPerVoxelPlane();

	InitializeEdit(IndexParams);
	
	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = {IndexParams.VoxelGrid, 0, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {IndexParams.VoxelGrid, VoxelLayer, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {IndexParams.VoxelGrid, ChunkDimension, 0};

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
			IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;

			// Edit Interval
			if (IndexParams.EditEnabled)
			{
				EditVoxelGrid(IndexParams);
			}

			AdvanceAllMeshingEvents(IndexParams, X, Y, Z);

			auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
			auto BorderSample = LeadingEvent.GetCurrentVoxel();

			// Left border
			AddLeftBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, BorderSample);

			//Back border
			AddBackBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, BorderSample);

			// Front border
			AddFrontBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, BorderSample);

			// Top border
			AddTopBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, BorderSample);

			// Bottom border
			AddBottomBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, BorderSample);

			CreateVirtualVoxelFacesInLShape(IndexParams, X, Y, Z);

			// Meshing event was finished
			IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
			Y += IndexParams.IndexSequenceBetweenEvents;

			IndexParams.PreviousVoxelRun = &IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
		}
		while (Y < ChunkDimension);

		// Right Border
		AddRightBorderSample(*IndexParams.SampledBorderChunks, X, Y, Z, *IndexParams.PreviousVoxelRun);
	}
}

void URLERunDirectionalVoxelMesher::AdvanceAllMeshingEvents(FIndexParams& IndexParams, int X, int Y, int Z)
{
	if (AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval))
	{
		const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
		const auto& LeadingMeshingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
		auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();

		// Left
		if ((IndexParams.PreviousVoxelRun->Voxel.IsEmptyVoxel() || IndexParams.PreviousVoxelRun->Voxel.IsTransparent() && 
			!LeadingMeshingEventVoxel.IsVoxelEmpty()) && Y != 0)
		{
			auto InitialPosition = FIntVector(X, Y, Z);
			CreateSideFace(*IndexParams.VirtualFaces[EFaceDirection::Left], FStaticMergeData::LeftFaceData, InitialPosition,
						   LeadingMeshingEventVoxel, IndexParams.IndexSequenceBetweenEvents);
		}

		// Right
		if (!IndexParams.PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.Voxel.IsEmptyVoxel() ||
				LeadingMeshingEventVoxel.Voxel.IsTransparent()) && IndexParams.InitialPosition.Y +
			IndexParams.IndexSequenceBetweenEvents != ChunkDimension)
		{
			CreateSideFace(*IndexParams.VirtualFaces[EFaceDirection::Right], FStaticMergeData::RightFaceData, IndexParams.InitialPosition,
						   *IndexParams.PreviousVoxelRun, IndexParams.IndexSequenceBetweenEvents + 1);
		}
	}

	// Calculate index
	// Smallest interval should always be increase of Y dimension

	AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingXInterval);
	AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingZInterval);
}

void URLERunDirectionalVoxelMesher::CreateVirtualVoxelFacesInLShape(FIndexParams& IndexParams, int X, int Y, int Z)
{
	auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
	
	auto& FollowingXEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval];
	auto& FollowingZEvent = IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval];
			
	// Sample interval
	// Move interval one step ahead if at the run end
	bool IsLeadingEmpty = LeadingEvent.GetCurrentVoxel().IsVoxelEmpty();
	bool IsFollowingXEmpty = FollowingXEvent.GetCurrentVoxel().IsVoxelEmpty();
	bool IsFollowingZEmpty = FollowingZEvent.GetCurrentVoxel().IsVoxelEmpty();

	// Generate culled faces
	if (!IsLeadingEmpty || !IsFollowingXEmpty || !IsFollowingZEmpty)
	{
		const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
		bool IsLeadingTransparent = LeadingEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingXTransparent = FollowingXEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingZTransparent = FollowingZEvent.GetCurrentVoxel().IsTransparent();
				
		const uint32 MaxYSequence = (ChunkDimension - Y) + IndexParams.CurrentMeshingEventIndex;
		IndexParams.TryUpdateNextMeshingEvent(MaxYSequence);

		IndexParams.IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;

		IndexParams.InitialPosition = FIntVector(X, Y, Z);

		// TODO: rewrite transparency
				
		CreateFace(IndexParams.VirtualFaces, FStaticMergeData::FrontFaceData, IndexParams.InitialPosition,
				   FollowingXEvent.GetCurrentVoxel(), IndexParams.IndexSequenceBetweenEvents,
				   X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
					   IsFollowingXTransparent)));

		CreateFace(IndexParams.VirtualFaces, FStaticMergeData::TopFaceData, IndexParams.InitialPosition,
				   FollowingZEvent.GetCurrentVoxel(), IndexParams.IndexSequenceBetweenEvents,
				   Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
					   IsFollowingZTransparent)));

		CreateFace(IndexParams.VirtualFaces, FStaticMergeData::BackFaceData, IndexParams.InitialPosition,
				   LeadingEvent.GetCurrentVoxel(), IndexParams.IndexSequenceBetweenEvents,
				   X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
					   IsFollowingXTransparent)));

		CreateFace(IndexParams.VirtualFaces, FStaticMergeData::BottomFaceData, IndexParams.InitialPosition,
				   LeadingEvent.GetCurrentVoxel(), IndexParams.IndexSequenceBetweenEvents,
				   Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
					   IsFollowingZTransparent)));
	}
	else
	{
		// Skip large empty sequences
		IndexParams.IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.CurrentMeshingEventIndex;
	}
}

void URLERunDirectionalVoxelMesher::InitializeEdit(FIndexParams& IndexParams)
{
	TSharedPtr<TArray<FRLEVoxel>> OldVoxelGrid = nullptr;

	// Set first run to trigger first condition in while loop
	if (!IndexParams.VoxelEdits->IsEmpty())
	{
		OldVoxelGrid = IndexParams.VoxelGrid;
		IndexParams.VoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		IndexParams.VoxelGrid->Reserve(OldVoxelGrid->Num() + IndexParams.VoxelEdits->Num());
		IndexParams.EditEnabled = true;

		auto VoxelEdit = IndexParams.VoxelEdits->Pop();
		int CopyVoxelRunIndex = -1;
		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();
		
		EditEventArray->Push(VoxelEdit.EditVoxel);
		IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent] = {EditEventArray, VoxelEdit.EditEventIndex, 0};

		CopyVoxelRunIndex++;
		auto CopyVoxel = (*OldVoxelGrid)[CopyVoxelRunIndex];
		uint32 Offset = 0;
		// First voxel in a chunk
		
		// TODO: fix and create test for this
		if (VoxelEdit.EditEventIndex == 0)
		{
			IndexParams.VoxelGrid->Add(VoxelEdit.EditVoxel);

			uint32 RemainingIndex = CopyVoxel.RunLenght;
			while (VoxelEdit.EditVoxel.RunLenght > RemainingIndex)
			{
				Offset += CopyVoxel.RunLenght;
				CopyVoxelRunIndex++;
				CopyVoxel = (*OldVoxelGrid)[CopyVoxelRunIndex];
				RemainingIndex += CopyVoxel.RunLenght;
			}
			
			AdvanceEditInterval(IndexParams);

			CopyVoxel.RunLenght = RemainingIndex - VoxelEdit.EditVoxel.RunLenght;

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
}

void URLERunDirectionalVoxelMesher::EditVoxelGrid(FIndexParams& IndexParams)
{
	auto& CopyEvent = IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent];
	auto& EditEvent = IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent];
	auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];

	if (EditEvent.LastEventIndex == IndexParams.CurrentMeshingEventIndex)
	{
		FVoxel CurrentVoxel;
		uint32 RemainingIndex;
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

		if (CopyEvent.VoxelGridPtr->IsValidIndex(CopyEvent.VoxelRunIndex + 1) && (*CopyEvent.VoxelGridPtr)[CopyEvent.
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

void URLERunDirectionalVoxelMesher::AdvanceEditInterval(FIndexParams& IndexParams) const
{
	auto& EditEvent = IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent];

	if (!IndexParams.VoxelEdits->IsEmpty())
	{
		auto VoxelEdit = IndexParams.VoxelEdits->Pop();
		EditEvent.LastEventIndex = VoxelEdit.EditEventIndex;
		// TODO: rewrite empty
		// must remain empty because only one edit can be active at time
		EditEvent.VoxelGridPtr->Empty();
		EditEvent.VoxelGridPtr->Push(VoxelEdit.EditVoxel);
	}
	else
	{
		EditEvent.LastEventIndex = VoxelData->GetVoxelCountPerChunk() + 1;
	}
}

void URLERunDirectionalVoxelMesher::BorderGeneration(
	const TSharedPtr<TArray<FProcMeshSectionVars>>& BorderChunkMeshData, TMap<int32, uint32>& BorderLocalVoxelTable,
	TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
	 TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples,
	 EFaceDirection FaceDirection)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Border generation - RLE RunDirectionalMeshing generation")
#endif

	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlane = VoxelData->GetVoxelCountPerVoxelPlane();

	TArray<FVirtualVoxelFace> FaceContainer;
	FaceContainer.Reserve(VoxelPlane);

	TArray<FVirtualVoxelFace> InverseFaceContainer;
	FaceContainer.Reserve(VoxelPlane);

	// Generate border
	auto& FaceTemplate = FaceTemplates[FaceDirection];
	auto InverseDirection = FaceTemplate.StaticMeshingData.InverseFaceDirection;
	auto& InverseFaceTemplate = FaceTemplates[InverseDirection];

	for (uint32 x = 0; x < ChunkDimension; x++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			AddBorderFace(FaceContainer, InverseFaceContainer, BorderVoxelSamples,  InversedBorderVoxelSamples, FaceTemplate, InverseFaceTemplate, x, y);
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

void URLERunDirectionalVoxelMesher::AddBorderFace(TArray<FVirtualVoxelFace>& FaceContainer,
                                                   TArray<FVirtualVoxelFace>& InverseFaceContainer,
                                                   TSharedPtr<TArray<FRLEVoxel>> BorderVoxelSamples,
													TSharedPtr<TArray<FRLEVoxel>> InversedBorderVoxelSamples,
                                                   const FMeshingDirections& FaceTemplate,
                                                   const FMeshingDirections& InverseFaceTemplate, int X, int Y) const
{
	const int ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();


	FIntVector QuadPosition;
	FIntVector IndexPosition;

	FaceTemplate.StaticMeshingData.BorderLocation(IndexPosition, QuadPosition, X, Y, ChunkDimension);
	auto Index = VoxelData->CalculateVoxelIndex(IndexPosition);

	// Smear border
	auto SampledVoxel = (*BorderVoxelSamples)[Index];
	auto InverseSampledVoxel = (*InversedBorderVoxelSamples)[Index];

	SmearVoxelBorder(SampledVoxel, *BorderVoxelSamples, Index);
	SmearVoxelBorder(InverseSampledVoxel, *InversedBorderVoxelSamples, Index);

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

void URLERunDirectionalVoxelMesher::CreateFace(const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
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

void URLERunDirectionalVoxelMesher::SampleLeftChunkBorder(FBorderSamples& SampledBorderChunks, TSharedPtr<TArray<FRLEVoxel>> VoxelGrid)
{
	FIndexParams IndexParams;
	IndexParams.VoxelGrid = VoxelGrid;
	
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();
	const uint32 VoxelLayer = VoxelData->GetVoxelCountPerVoxelPlane();
	IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;
	
	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = {IndexParams.VoxelGrid, 0, 0};
	
	for (uint32 X = 0; X < ChunkDimension; X++)
	{
		for (uint32 Y = 0; Y < ChunkDimension; Y++)
		{
			uint32 NextIndex = VoxelData->CalculateVoxelIndex(X, 0, Y);
			IndexParams.TryUpdateNextMeshingEvent(NextIndex);
			do 
			{
				AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval);
			} while (IndexParams.CurrentMeshingEventIndex < MaxChunkVoxelSequence);
			
			IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;
			auto& MeshingEventVoxel = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
			AddRightBorderSample(SampledBorderChunks, X, 0, Y, MeshingEventVoxel);	
		}
	}
}

void URLERunDirectionalVoxelMesher::AddLeftBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y, const int Z, const FRLEVoxel& BorderSample) const
{
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, X, Z));
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Left, BorderSample, 1, Y == 0);
}

void URLERunDirectionalVoxelMesher::AddRightBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample) const
{
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, X, Z));
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Right, BorderSample, 1, true);
}

void URLERunDirectionalVoxelMesher::AddTopBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample) const
{
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, Y, X));
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Top, BorderSample, ChunkDimension - Y, Z == ChunkDimension - 1);
}

void URLERunDirectionalVoxelMesher::AddBottomBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample) const
{	
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, Y, X));
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Bottom, BorderSample, ChunkDimension - Y, Z == 0);
}

void URLERunDirectionalVoxelMesher::AddFrontBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample) const
{
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, Y, Z));
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Front, BorderSample, BorderSample.RunLenght, X == ChunkDimension - 1);
}

void URLERunDirectionalVoxelMesher::AddBackBorderSample(FBorderSamples& SampledBorderChunks, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample) const
{
	const uint32 VoxelPlaneIndex =  VoxelData->CalculateVoxelIndex(FIntVector(0, Y, Z));
	SampledBorderChunks.AddBorderSample(VoxelPlaneIndex, EFaceDirection::Back, BorderSample, BorderSample.RunLenght, X == 0);
}
