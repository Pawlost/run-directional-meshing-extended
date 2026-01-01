#include "VoxelMesher/RLERunDirectionalVoxelMesher.h"
#include "VoxelMesher/RunDirectionalVoxelMesher.h"
#include "Voxel/RLEVoxel.h"
#include "VoxelModel/RLEVoxelGrid.h"

void URLERunDirectionalVoxelMesher::CompressVoxelModel(TArray<FVoxel>& VoxelGrid)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE compression generation")
#endif

	if (VoxelGrid.Num() != 0 && RLEVoxelGrid == nullptr)
	{
		auto NewRLEVoxelGrid = MakeShared<TArray<FRLEVoxel>>();

		// Seed with first voxel run
		NewRLEVoxelGrid->Emplace(1, VoxelGrid[0]);

		for (int32 x = 1; x < VoxelGrid.Num(); x++)
		{
			const FVoxel& Voxel = VoxelGrid[x];
			if (NewRLEVoxelGrid->Last().Voxel == Voxel)
			{
				NewRLEVoxelGrid->Last().RunLenght++;
			}
			else
			{
				NewRLEVoxelGrid->Emplace(1, Voxel);
			}
		}

		RLEVoxelGrid = NewRLEVoxelGrid;
	}
}

// persistent preallocation must be maintained
void URLERunDirectionalVoxelMesher::GenerateMesh(
                                                 TStaticArray<
	                                                 TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>&
                                                 VirtualFaces,
                                                 TMap<int32, uint32>& LocalVoxelTable,
                                                 TSharedPtr<TArray<FProcMeshSectionVars>>& ChunkMeshData,
                                                 TArray<FRLEVoxelEdit>& VoxelChanges,
                                                 TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>
                                                 SideFaces,
                                                 TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>&
                                                 SideMeshers, FBorderSamples& BorderSamples)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Total - RLE RunDirectionalMeshing generation")
#endif

	FaceGeneration(VoxelChanges, VirtualFaces, SideFaces, SideMeshers);

	const uint32 ChunkDimension = VoxelData->GetVoxelCountPerVoxelLine();
	
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - Directional Greedy Merge");
#endif
		
		constexpr int EstimatedRows = 3;
		
		TArray<FVirtualVoxelFace> FirstArray;
		FirstArray.Reserve(ChunkDimension * EstimatedRows);

		TArray<FVirtualVoxelFace> SecondArray;
		SecondArray.Reserve(ChunkDimension * EstimatedRows);

		for (int f = 0; f < CHUNK_FACE_COUNT; f++)
		{
			for (uint32 y = 0; y < ChunkDimension; y++)
			{
				DirectionalGreedyMerge(*ChunkMeshData, FirstArray, SecondArray, LocalVoxelTable,
				                       FaceTemplates[f].StaticMeshingData, (*VirtualFaces[f])[y]);
			}

			DirectionalGreedyMerge(*ChunkMeshData, FirstArray, SecondArray, LocalVoxelTable,
			                       FaceTemplates[f].StaticMeshingData, *SideFaces[f]);
		}
	}
}

void URLERunDirectionalVoxelMesher::TraverseYDirection(FIndexParams& IndexParams, uint32 X, uint32 Y, uint32 Z,
                                                       TStaticArray<
	                                                       TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>&
                                                       SideMeshers,
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

		CreateVirtualVoxelFacesInLShape(IndexParams, SideMeshers, BorderIndexParams, X, Y, Z);

		// Meshing event was finished
		IndexParams.CurrentMeshingEventIndex = IndexParams.NextMeshingEventIndex;
		Y += IndexParams.IndexSequenceBetweenEvents;

		IndexParams.PreviousVoxelRun = &IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval].
			GetCurrentVoxel();
	}
	while (Y < ChunkDimension);

	if (!IndexParams.PreviousVoxelRun->IsVoxelEmpty())
	{
		// Right Border
		CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, *IndexParams.PreviousVoxelRun, 1,
		             EFaceDirection::Right, X, 0, Z, true);
	}

	if (IndexParams.EditEnabled)
	{
		RLEVoxelGrid = IndexParams.MeshingEvents[LeadingInterval].VoxelGridPtr;
	}
}

void URLERunDirectionalVoxelMesher::FaceGeneration(TArray<FRLEVoxelEdit>& VoxelEdits,
                                                   TStaticArray<
	                                                   TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>&
                                                   VirtualFaces,
                                                   TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>
                                                   &
                                                   SideFaces,
                                                   TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>&
                                                   SideMeshers)
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

	auto VoxelGrid = InitializeEdit(IndexParams);

	IndexParams.MeshingEvents[EMeshingEventIndex::LeadingInterval] = {VoxelGrid, 0, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {VoxelGrid, VoxelLayer, 0};
	IndexParams.MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {VoxelGrid, ChunkDimension, 0};

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
		if ((IndexParams.PreviousVoxelRun->Voxel.IsEmptyVoxel() || IndexParams.PreviousVoxelRun->Voxel.IsTransparent()
			&&
			!LeadingMeshingEventVoxel.IsVoxelEmpty()) && Y != 0)
		{
			auto InitialPosition = FIntVector(X, Y, Z);
			CreateSideFace(*IndexParams.VirtualFaces[EFaceDirection::Left], FStaticMergeData::LeftFaceData,
			               InitialPosition,
			               LeadingMeshingEventVoxel, IndexParams.IndexSequenceBetweenEvents);
		}

		// Right
		if (!IndexParams.PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.Voxel.IsEmptyVoxel() ||
				LeadingMeshingEventVoxel.Voxel.IsTransparent()) && IndexParams.InitialPosition.Y +
			IndexParams.IndexSequenceBetweenEvents != ChunkDimension)
		{
			CreateSideFace(*IndexParams.VirtualFaces[EFaceDirection::Right], FStaticMergeData::RightFaceData,
			               IndexParams.InitialPosition,
			               *IndexParams.PreviousVoxelRun, IndexParams.IndexSequenceBetweenEvents + 1);
		}
	}

	// Calculate index
	// Smallest interval should always be increase of Y dimension

	AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingXInterval);
	AdvanceMeshingEvent(IndexParams, EMeshingEventIndex::FollowingZInterval);
}

void URLERunDirectionalVoxelMesher::CreateVirtualVoxelFacesInLShape(FIndexParams& IndexParams,
                                                                    TStaticArray<
	                                                                    TStrongObjectPtr<UVoxelMesherBase>,
	                                                                    CHUNK_FACE_COUNT>& SideMeshers,
                                                                    TStaticArray<FIndexParams, CHUNK_FACE_COUNT>&
                                                                    BorderIndexParams,
                                                                    int X, int Y, int Z)
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
		IndexParams.IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.
			CurrentMeshingEventIndex;

		auto& LeadingEventVoxel = LeadingEvent.GetCurrentVoxel();
		if (!IsLeadingEmpty)
		{
			// Left border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, LeadingEventVoxel, 1,
			             EFaceDirection::Left, X, ChunkDimension - 1, Z, Y == 0);

			// Front border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X + 1, Y, Z, LeadingEventVoxel,
			             IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Front, 0, Y, Z, X == ChunkDimension - 1);

			// Top border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z + 1, LeadingEventVoxel,
			             IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Top, X, Y, 0, Z == ChunkDimension - 1);

			// Bottom border
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, LeadingEventVoxel,
			             IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Bottom, X, Y, ChunkDimension - 1,
			             Z == 0);

			//Back borders
			CreateBorder(IndexParams.SideFaces, SideMeshers, BorderIndexParams, X, Y, Z, LeadingEventVoxel,
			             IndexParams.IndexSequenceBetweenEvents, EFaceDirection::Back, ChunkDimension - 1, Y, Z,
			             X == 0);
		}

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
		           LeadingEventVoxel, IndexParams.IndexSequenceBetweenEvents,
		           X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
			           IsFollowingXTransparent)));

		CreateFace(IndexParams.VirtualFaces, FStaticMergeData::BottomFaceData, IndexParams.InitialPosition,
		           LeadingEventVoxel, IndexParams.IndexSequenceBetweenEvents,
		           Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
			           IsFollowingZTransparent)));
	}
	else
	{
		// Skip large empty sequences
		IndexParams.IndexSequenceBetweenEvents = IndexParams.NextMeshingEventIndex - IndexParams.
			CurrentMeshingEventIndex;
	}
}

TSharedPtr<TArray<FRLEVoxel>> URLERunDirectionalVoxelMesher::InitializeEdit(FIndexParams& IndexParams)
{
	// Set first run to trigger first condition in while loop
	if (!IndexParams.VoxelEdits->IsEmpty())
	{
		auto NewVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		NewVoxelGrid->Reserve(RLEVoxelGrid->Num() + IndexParams.VoxelEdits->Num());
		IndexParams.EditEnabled = true;

		auto VoxelEdit = IndexParams.VoxelEdits->Pop();
		int CopyVoxelRunIndex = -1;
		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();

		EditEventArray->Push(VoxelEdit.EditVoxel);
		IndexParams.MeshingEvents[EMeshingEventIndex::EditEvent] = {EditEventArray, VoxelEdit.EditEventIndex, 0};

		CopyVoxelRunIndex++;
		auto CopyVoxel = (*RLEVoxelGrid)[CopyVoxelRunIndex];
		uint32 Offset = 0;
		// First voxel in a chunk

		// TODO: fix and create test for this
		if (VoxelEdit.EditEventIndex == 0)
		{
			NewVoxelGrid->Add(VoxelEdit.EditVoxel);

			uint32 RemainingIndex = CopyVoxel.RunLenght;
			while (VoxelEdit.EditVoxel.RunLenght > RemainingIndex)
			{
				Offset += CopyVoxel.RunLenght;
				CopyVoxelRunIndex++;
				CopyVoxel = (*RLEVoxelGrid)[CopyVoxelRunIndex];
				RemainingIndex += CopyVoxel.RunLenght;
			}

			AdvanceEditInterval(IndexParams);

			CopyVoxel.RunLenght = RemainingIndex - VoxelEdit.EditVoxel.RunLenght;

			if (NewVoxelGrid->Last().Voxel == CopyVoxel.Voxel)
			{
				NewVoxelGrid->Last().RunLenght += CopyVoxel.RunLenght;
			}
			else
			{
				NewVoxelGrid->Add(CopyVoxel);
			}
		}
		else
		{
			NewVoxelGrid->Add(CopyVoxel);
		}

		IndexParams.MeshingEvents[EMeshingEventIndex::CopyEvent] = {RLEVoxelGrid, Offset, CopyVoxelRunIndex};
		return NewVoxelGrid;
	}

	return RLEVoxelGrid;
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
		FRLEVoxel CopyVoxel;

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

		if (LeadingEvent.VoxelGridPtr->Last().Voxel == EditVoxel.Voxel)
		{
			LeadingEvent.VoxelGridPtr->Last().RunLenght += EditVoxel.RunLenght;
		}
		else
		{
			LeadingEvent.VoxelGridPtr->Add(EditVoxel);
		}

		RemainingIndex -= EditVoxel.RunLenght;

		if (LeadingEvent.VoxelGridPtr->Last().Voxel == CurrentVoxel)
		{
			LeadingEvent.VoxelGridPtr->Last().RunLenght += RemainingIndex;
		}
		else if (RemainingIndex != 0)
		{
			LeadingEvent.VoxelGridPtr->Add(FRLEVoxel{RemainingIndex, CurrentVoxel});
		}

		if (CopyEvent.VoxelGridPtr->IsValidIndex(CopyEvent.VoxelRunIndex + 1) && (*CopyEvent.VoxelGridPtr)[CopyEvent.
			VoxelRunIndex + 1].Voxel == LeadingEvent.VoxelGridPtr->Last().Voxel)
		{
			CopyEvent.AdvanceEvent();
			LeadingEvent.VoxelGridPtr->Last().RunLenght += CopyEvent.GetCurrentVoxel().RunLenght;
		}

		AdvanceEditInterval(IndexParams);
	}
	else
	{
		int CopyEventIndex = CopyEvent.GetEventIndex();
		if (CopyEventIndex == IndexParams.CurrentMeshingEventIndex)
		{
			CopyEvent.AdvanceEvent();
			LeadingEvent.VoxelGridPtr->Add(CopyEvent.GetCurrentVoxel());
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

void URLERunDirectionalVoxelMesher::CreateFace(
	const TStaticArray<TSharedPtr<TArray<TArray<FVirtualVoxelFace>>>, CHUNK_FACE_COUNT>& VirtualFaces,
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

void URLERunDirectionalVoxelMesher::CreateBorder(
	TStaticArray<TSharedPtr<TArray<FVirtualVoxelFace>>, CHUNK_FACE_COUNT>& SideFaces,
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	TStaticArray<FIndexParams, CHUNK_FACE_COUNT>& BorderIndexParams,
	const int X, const int Y, const int Z, const FRLEVoxel& BorderSample,
	int IndexInBetweenIntervals, EFaceDirection Direction,
	const int BorderX, const int BorderY, const int BorderZ, bool BorderCondition)
{
	if (BorderCondition)
	{
		// TODO: pass as parameter
		constexpr bool ShowBorders = true;
		auto Mesher = SideMeshers[Direction];
		if (ShowBorders || Mesher != nullptr)
		{
			for (int y = 0; y < IndexInBetweenIntervals; y++)
			{
				if ((ShowBorders && Mesher == nullptr) || Mesher->IsBorderVoxelEmpty(BorderIndexParams[Direction], BorderX, BorderY + y, BorderZ))
				{
					auto StaticData = FaceTemplates[Direction].StaticMeshingData;
					const FVirtualVoxelFace NewFace = StaticData.FaceCreator(
						BorderSample.Voxel, FIntVector(X, Y + y, Z), 1);
					AddFace(StaticData, NewFace, *SideFaces[Direction]);
				}
			}
		}
	}
}
