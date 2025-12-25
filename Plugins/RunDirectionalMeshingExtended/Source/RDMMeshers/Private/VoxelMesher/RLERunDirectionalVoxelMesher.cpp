#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif
	
	RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

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
}

// persistent preallocation must be maintained
void URLERunDirectionalVoxelMesher::GenerateMesh(const TStrongObjectPtr<UVoxelModel>& VoxelModel,
							  TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
							  TMap<int32, uint32>& LocalVoxelTable,
							  TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
							  TArray<FRLEVoxelEdit>& VoxelChanges,
							  TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,
							  TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers, FBorderSamples& BorderSamples)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	FaceGeneration(VoxelChanges, VirtualFaces, SideFaces, SideMeshers);
	
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < ChunkDimension; y++)
		{
			DirectionalGreedyMerge(*ChunkMeshData, LocalVoxelTable,
								   FaceTemplates[f].StaticMeshingData, (*VirtualFaces[f])[y]);
		}
		
		DirectionalGreedyMerge(*ChunkMeshData, LocalVoxelTable,
							   FaceTemplates[f].StaticMeshingData, *SideFaces[f]);
	}
}

void URLERunDirectionalVoxelMesher::TraverseYDirection(FIndexParams& IndexParams, uint32 X, uint32 Y, uint32 Z, 	
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	TStaticArray<FIndexParams, CHUNK_FACE_COUNT>& BorderIndexParams)
{
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();
	
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
			
		CreateVirtualVoxelFacesInLShape(IndexParams, X, Y, Z);

		auto& LeadingEvent = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval];
		auto BorderSample = LeadingEvent.GetCurrentVoxel();

		if(!BorderSample.IsVoxelEmpty())
		{
			// Left border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, BorderSample, 1, EFaceDirection::Left, X, ChunkDimension - 1, Z, Y == 0);
			
			// Front border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X + 1, Y, Z, BorderSample, IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Front, 0, Y, Z, Z == 0);
			
			// Top border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z + 1, BorderSample, IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Top, X, Y, 0, Z == ChunkDimension - 1);
			
			// Bottom border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, BorderSample, IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Bottom, X, Y, ChunkDimension - 1, Z == 0);
			
			//Back borders
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, BorderSample, IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Back, ChunkDimension - 1, Y, Z, X == 0);
		}
		
		// Meshing event was finished
		IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
		Y += IndexParams.IndexSequenceBetweenEvents;

		if(!BorderSample.IsVoxelEmpty())
		{
			// Right Border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, BorderSample,1, EFaceDirection::Right, X, 0, Z, Y == ChunkDimension);
		}
		
		IndexParams.PreviousVoxelRun = &IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
	}
	while (Y < ChunkDimension);
}

void URLERunDirectionalVoxelMesher::FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits, 
TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT> VirtualFaces,
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT> SideFaces,	
TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif
	
	// TODO: make private class member, problematic writing because of parallel thread execution, maybe should be wrapped in a class
	FIndexParams IndexParams;
	
	IndexParams.VoxelEdits = &VoxelEdits;
	IndexParams.VirtualFaces = VirtualFaces;
	IndexParams.SideFaces = SideFaces;
	
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();
	const uint32 VoxelLayer = VoxelData->GetVoxelCountPerVoxelPlane();
	
	TStaticArray<FIndexParams, CHUNK_FACE_COUNT> BorderIndexParams;
	
	InitializeEdit(IndexParams);
	
	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = { RLEVoxelGrid, 0, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {RLEVoxelGrid, VoxelLayer, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {RLEVoxelGrid, ChunkDimension, 0};

	// Traverse through voxel grid
	while (IndexParams.CurrentMeshingEventIndex < MaxChunkVoxelSequence)
	{
		const uint32 X = IndexParams.CurrentMeshingEventIndex / (VoxelLayer);
		const uint32 Z = ((IndexParams.CurrentMeshingEventIndex / ChunkDimension) % ChunkDimension);
		uint32 Y = IndexParams.CurrentMeshingEventIndex % ChunkDimension;
		
		TraverseYDirection(IndexParams, X, Y, Z, SideMeshers, BorderIndexParams);
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
		OldVoxelGrid = RLEVoxelGrid;
		RLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		RLEVoxelGrid->Reserve(OldVoxelGrid->Num() + IndexParams.VoxelEdits->Num());
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
			RLEVoxelGrid->Add(VoxelEdit.EditVoxel);

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

			if (RLEVoxelGrid->Last().Voxel == CopyVoxel.Voxel)
			{
				RLEVoxelGrid->Last().RunLenght += CopyVoxel.RunLenght;
			}else
			{
				RLEVoxelGrid->Add(CopyVoxel);
			}
		}else
		{
			RLEVoxelGrid->Add(CopyVoxel);
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

		if (RLEVoxelGrid->Last().Voxel == EditVoxel.Voxel)
		{
			RLEVoxelGrid->Last().RunLenght += EditVoxel.RunLenght;
		}
		else
		{
			RLEVoxelGrid->Add(EditVoxel);
		}

		RemainingIndex -= EditVoxel.RunLenght;

		if (RLEVoxelGrid->Last().Voxel == CurrentVoxel)
		{
			RLEVoxelGrid->Last().RunLenght += RemainingIndex;
		}
		else if (RemainingIndex != 0)
		{
			RLEVoxelGrid->Add(FRLEVoxel{RemainingIndex, CurrentVoxel});
		}

		if (CopyEvent.VoxelGridPtr->IsValidIndex(CopyEvent.VoxelRunIndex + 1) && (*CopyEvent.VoxelGridPtr)[CopyEvent.
			VoxelRunIndex + 1].Voxel == RLEVoxelGrid->Last().Voxel)
		{
			CopyEvent.AdvanceEvent();
			RLEVoxelGrid->Last().RunLenght += CopyEvent.GetCurrentVoxel().RunLenght;
		}

		AdvanceEditInterval(IndexParams);
	}
	else
	{
		int CopyEventIndex = CopyEvent.GetEventIndex();
		if (CopyEventIndex == IndexParams.CurrentMeshingEventIndex)
		{
			CopyEvent.AdvanceEvent();
			RLEVoxelGrid->Add(CopyEvent.GetCurrentVoxel());
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

bool URLERunDirectionalVoxelMesher::IsBorderVoxelEmpty(FIndexParams& IndexParams, int X, int Y, int Z)
{
	const uint32 MaxChunkVoxelSequence = VoxelData->GetVoxelCountPerChunk();
	
	auto& VoxelGridPtr = IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].VoxelGridPtr;
	if (VoxelGridPtr == nullptr)
	{
		VoxelGridPtr = RLEVoxelGrid;
	}
	
	uint32 NextIndex = VoxelData->CalculateVoxelIndex(X, Y, Z);
	
	while (IndexParams.CurrentMeshingEventIndex < NextIndex)
	{
		IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
		IndexParams.NextMeshingEventIndex = MaxChunkVoxelSequence;
		AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::LeadingInterval);
		IndexParams.TryUpdateNextMeshingEvent(NextIndex);
	} 
	
	return IndexParams.MeshingEvents[LeadingInterval].GetCurrentVoxel().Voxel.IsEmptyVoxel();
}

void URLERunDirectionalVoxelMesher::AddLeftBorderSample(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample, int IndexInBetweenIntervals) const
{
	if (Y == 0)
	{
		auto Mesher = SideMeshers[Left];
	
		const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
		
		if (Mesher != nullptr && !Mesher->IsBorderVoxelEmpty(BorderIndexParams, X, ChunkDimension - 1, Z))
		{
			return;
		}
	
		auto StaticData = FaceTemplates[Left].StaticMeshingData;
		const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X, Y, Z), 1);
		AddFace(StaticData, NewFace, *SideFaces[Left]);
	}
}

void URLERunDirectionalVoxelMesher::AddRightBorderSample(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample, int IndexInBetweenIntervals) const
{
	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	
	if (Y == ChunkDimension)
	{
		auto Mesher = SideMeshers[Right];
		
		if (Mesher != nullptr && !Mesher->IsBorderVoxelEmpty(BorderIndexParams, X, 0, Z) )
		{
			return;
		}
		
		auto StaticData = FaceTemplates[Right].StaticMeshingData;
		const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X, Y, Z), 1);
		AddFace(StaticData, NewFace, *SideFaces[Right]);
	}
}

void URLERunDirectionalVoxelMesher::AddTopBorderSample(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample, int IndexInBetweenIntervals) const
{
	const int ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	if (Z == ChunkDimension - 1){
		auto Mesher = SideMeshers[Top];
		for (int y = 0; y < (ChunkDimension - Y); y++)
		{
    		if (Mesher != nullptr && !Mesher->IsBorderVoxelEmpty(BorderIndexParams, X, Y + y, 0) )
    		{
    			continue;
    		}
    		
    		auto StaticData = FaceTemplates[Top].StaticMeshingData;
    		const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X, Y + y, Z + 1), 1);
    		AddFace(StaticData, NewFace, *SideFaces[Top]);
    	}
	}
}

void URLERunDirectionalVoxelMesher::AddBottomBorderSample(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams,const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample, int IndexInBetweenIntervals) const
{	
	if (Z == 0){
		const int ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
		auto Mesher = SideMeshers[Bottom];
		for (int y = 0; y < ChunkDimension - Y; y++)
		{
			if (Mesher != nullptr && !Mesher->IsBorderVoxelEmpty(BorderIndexParams, X, Y + y, ChunkDimension - 1) )
			{
				continue;
			}
    		
			auto StaticData = FaceTemplates[Bottom].StaticMeshingData;
			const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X, Y + y, Z), 1);
			AddFace(StaticData, NewFace, *SideFaces[Bottom]);
		}
	}
}

void URLERunDirectionalVoxelMesher::AddFrontBorderSample(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		FIndexParams& BorderIndexParams, const int X, const int Y,
	const int Z, const FRLEVoxel& BorderSample, int IndexInBetweenIntervals) const
{
	const int ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	if (X == ChunkDimension - 1){
		auto Mesher = SideMeshers[Front];
		for (int y = 0; y < ChunkDimension - Y; y++)
		{
			if (Mesher != nullptr && !Mesher->IsBorderVoxelEmpty(BorderIndexParams, 0, Y + y, Z))
			{
				continue;
			}
    		
			auto StaticData = FaceTemplates[Front].StaticMeshingData;
			const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X + 1, Y + y, Z), 1);
			AddFace(StaticData, NewFace, *SideFaces[Front]);
		}
	}
}


void URLERunDirectionalVoxelMesher::CreateBorder(
		TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
		TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
		TStaticArray<FIndexParams, CHUNK_FACE_COUNT> BorderIndexParams,
		const int X, const int Y, const int Z, const FRLEVoxel& BorderSample, 
		int IndexInBetweenIntervals, EFaceDirection Direction,
		const int BorderX, const int BorderY, const int BorderZ, bool BorderCondition)
{
	auto Mesher = SideMeshers[Direction];
	if (BorderCondition && Mesher != nullptr)
	{
		for (int y = 0; y < IndexInBetweenIntervals; y++)
		{
			if (!Mesher->IsBorderVoxelEmpty(BorderIndexParams[Direction], BorderX, BorderY + y, BorderZ))
			{
				continue;
			}
			
			auto StaticData = FaceTemplates[Direction].StaticMeshingData;
			const FVirtualVoxelFace NewFace = StaticData.FaceCreator(BorderSample.Voxel, FIntVector(X, Y + y, Z), 1);
			AddFace(StaticData, NewFace, *SideFaces[Direction]);
		}
	}	
}
