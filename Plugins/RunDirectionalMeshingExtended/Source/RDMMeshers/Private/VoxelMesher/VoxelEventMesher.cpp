#include "VoxelMesher/VoxelEventMesher.h"
#include "VoxelMesher/VirtualChunk/ChunkBorderContext.h"

void FVoxelEventMesher::AdvanceEditInterval(TArray<FRLEVoxelEdit>& VoxelEdits)
{
	auto& EditEvent = MeshingEvents[EMeshingEventIndex::EditEvent];

	constexpr int Offset = 1;
	EditEvent.LastEventIndex = MaxVoxelsInChunk + Offset;

	if (!VoxelEdits.IsEmpty())
	{
		const auto& [EditEventIndex, EditVoxel] = VoxelEdits.Pop(EAllowShrinking::No);
		EditEvent.LastEventIndex = EditEventIndex;
		// must remain empty because only one edit can be active at time
		(*EditEvent.VoxelGridPtr)[0] = EditVoxel;
	}
}

FVoxelEventMesher::FVoxelEventMesher(const TObjectPtr<UBaseVoxelData> BaseVoxelData)
	: FVoxelEventMesherBase(BaseVoxelData)
{
	for (uint32 y = 0; y < VoxelLine; y++)
	{
		VirtualFaces.Emplace(VoxelPlane);
	}
}

void FVoxelEventMesher::UpdateInternalState(const uint32 VoxelLineParam, const uint32 VoxelPlaneParam,
                                            const uint32 MaxVoxelsInChunkParam)
{
	VoxelLine = VoxelLineParam;
	VoxelPlane = VoxelPlaneParam;
	MaxVoxelsInChunk = MaxVoxelsInChunkParam;

	InternalReset();
}

void FVoxelEventMesher::InitializeIntervals(const TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid,
                                            TArray<FRLEVoxelEdit>& VoxelEdits)
{
	TSharedPtr<TArray<FRLEVoxel>> MainVoxelGrid;
	// Set first run to trigger first condition in while loop
	if (!VoxelEdits.IsEmpty())
	{
		const auto NewVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		constexpr int PreallocationEstimation = 3;
		NewVoxelGrid->Reserve(RLEVoxelGrid->Num() + VoxelEdits.Num() * PreallocationEstimation);
		EditEnabled = true;

		const auto [EditEventIndex, EditVoxel] = VoxelEdits.Pop();
		int CopyVoxelRunIndex = -1;
		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();

		EditEventArray->Push(EditVoxel);
		MeshingEvents[EMeshingEventIndex::EditEvent] = {EditEventArray, EditEventIndex, 0};

		CopyVoxelRunIndex++;
		auto CopyVoxel = (*RLEVoxelGrid)[CopyVoxelRunIndex];
		uint32 Offset = 0;
		// First voxel in a chunk

		// TODO: fix and create test for this
		if (EditEventIndex == 0)
		{
			NewVoxelGrid->Add(EditVoxel);

			uint32 RemainingIndex = CopyVoxel.RunLenght;
			while (EditVoxel.RunLenght > RemainingIndex)
			{
				Offset += CopyVoxel.RunLenght;
				CopyVoxelRunIndex++;
				CopyVoxel = (*RLEVoxelGrid)[CopyVoxelRunIndex];
				RemainingIndex += CopyVoxel.RunLenght;
			}

			AdvanceEditInterval(VoxelEdits);

			CopyVoxel.RunLenght = RemainingIndex - EditVoxel.RunLenght;

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

		MeshingEvents[EMeshingEventIndex::CopyEvent] = {RLEVoxelGrid, Offset, CopyVoxelRunIndex};
		MainVoxelGrid = NewVoxelGrid;
	}
	else
	{
		MainVoxelGrid = RLEVoxelGrid;
	}

	MeshingEvents[EMeshingEventIndex::LeadingInterval] = {MainVoxelGrid, 0, 0};
	MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {MainVoxelGrid, VoxelPlane, 0};
	MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {MainVoxelGrid, VoxelLine, 0};
}

void FVoxelEventMesher::GenerateVirtualFaces(FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
#endif

	// Traverse through voxel grid
	while (VoxelIndex < MaxVoxelsInChunk)
	{
		UpdatePositionFromIndex();

		TraverseYDirection(BorderParameters, VoxelEdits);
	}
}

void FVoxelEventMesher::TraverseYDirection(FChunkBorderContext& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
{
	// Check borders
	do
	{
		// Reset interval flag
		NextVoxelIndex = MaxVoxelsInChunk;

		// Edit Interval
		if (EditEnabled)
		{
			EditVoxelGrid(VoxelEdits);
		}

		AdvanceAllMeshingEvents();

		CreateVirtualVoxelFacesInLShape(BorderParameters);

		// Meshing event was finished
		VoxelIndex = NextVoxelIndex;
		VoxelPosition.Y += IndexSequenceBetweenEvents;

		PreviousVoxelRun = &MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentRLEVoxel();
	}
	while (static_cast<uint32>(VoxelPosition.Y) < VoxelLine);

	if (!PreviousVoxelRun->IsVoxelEmpty())
	{
		VoxelPosition.Y--;
		// Right Border
		CreateBorder(BorderParameters, 1, *PreviousVoxelRun,
		             EFaceDirection::Right, FIntVector(VoxelPosition.X, VoxelLine - 1, VoxelPosition.Z), true);
	}
}


void FVoxelEventMesher::CreateVirtualVoxelFacesInLShape(FChunkBorderContext& BorderParameters)
{
	const auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	const auto& FollowingXEvent = MeshingEvents[EMeshingEventIndex::FollowingXInterval];
	const auto& FollowingZEvent = MeshingEvents[EMeshingEventIndex::FollowingZInterval];

	// Sample interval
	// Move interval one step ahead if at the run end
	const bool IsLeadingEmpty = LeadingEvent.GetCurrentRLEVoxel().IsVoxelEmpty();
	const bool IsFollowingXEmpty = FollowingXEvent.GetCurrentRLEVoxel().IsVoxelEmpty();
	const bool IsFollowingZEmpty = FollowingZEvent.GetCurrentRLEVoxel().IsVoxelEmpty();

	// Generate culled faces
	if (!IsLeadingEmpty || !IsFollowingXEmpty || !IsFollowingZEmpty)
	{
		const bool IsLeadingTransparent = LeadingEvent.GetCurrentVoxel().IsTransparent();
		const bool IsFollowingXTransparent = FollowingXEvent.GetCurrentVoxel().IsTransparent();
		const bool IsFollowingZTransparent = FollowingZEvent.GetCurrentVoxel().IsTransparent();

		const uint32 MaxYSequence = (VoxelLine - VoxelPosition.Y) + VoxelIndex;
		TryUpdateNextMeshingEvent(MaxYSequence);
		IndexSequenceBetweenEvents = NextVoxelIndex - VoxelIndex;

		auto& LeadingEventVoxel = LeadingEvent.GetCurrentRLEVoxel();
		if (!IsLeadingEmpty)
		{
			// Left border
			CreateBorder(BorderParameters, 1, LeadingEventVoxel,
			             EFaceDirection::Left, FIntVector(VoxelPosition.X, 0, VoxelPosition.Z), VoxelPosition.Y == 0);

			// Front border
			CreateBorder(BorderParameters, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Front, FIntVector(0, VoxelPosition.Y, VoxelPosition.Z),
			             VoxelPosition.X == VoxelLine - 1);

			// Top border
			CreateBorder(BorderParameters, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Top, FIntVector(VoxelPosition.X, VoxelPosition.Y, 0),
			             VoxelPosition.Z == VoxelLine - 1);

			// Bottom border
			CreateBorder(BorderParameters, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Bottom, FIntVector(VoxelPosition.X, VoxelPosition.Y, VoxelLine - 1),
			             VoxelPosition.Z == 0);

			//Back borders
			CreateBorder(BorderParameters, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Back, FIntVector(VoxelLine - 1, VoxelPosition.Y, VoxelPosition.Z),
			             VoxelPosition.X == 0);
		}

		PreviousPosition = VoxelPosition;

		if (VoxelPosition.X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			IsFollowingXTransparent)))
		{
			VirtualFaces[VoxelPosition.X].AddNewVirtualFace(EFaceDirection::Front, FollowingXEvent.GetCurrentVoxel(),
			                                                VoxelPosition, IndexSequenceBetweenEvents);
		}

		if (VoxelPosition.Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			IsFollowingZTransparent)))
		{
			VirtualFaces[VoxelPosition.Z].AddNewVirtualFace(EFaceDirection::Top, FollowingZEvent.GetCurrentVoxel(),
			                                                VoxelPosition, IndexSequenceBetweenEvents);
		}

		if (VoxelPosition.X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
			IsFollowingXTransparent)))
		{
			VirtualFaces[VoxelPosition.X].AddNewVirtualFace(EFaceDirection::Back, LeadingEventVoxel.Voxel,
			                                                VoxelPosition, IndexSequenceBetweenEvents);
		}

		if (VoxelPosition.Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
			IsFollowingZTransparent)))
		{
			VirtualFaces[VoxelPosition.Z].AddNewVirtualFace(EFaceDirection::Bottom, LeadingEventVoxel.Voxel,
			                                                VoxelPosition, IndexSequenceBetweenEvents);
		}
	}
	else
	{
		// Skip large empty sequences
		IndexSequenceBetweenEvents = NextVoxelIndex - VoxelIndex;
	}
}

void FVoxelEventMesher::EditVoxelGrid(TArray<FRLEVoxelEdit>& VoxelEdits)
{
	auto& CopyEvent = MeshingEvents[EMeshingEventIndex::CopyEvent];
	const auto& EditEvent = MeshingEvents[EMeshingEventIndex::EditEvent];
	const auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];

	if (EditEvent.LastEventIndex == VoxelIndex)
	{
		FVoxel CurrentVoxel;
		uint32 RemainingIndex;
		FRLEVoxel CopyVoxel;

		if (EditEvent.LastEventIndex == LeadingEvent.GetEventIndex())
		{
			CopyEvent.AdvanceEvent();
			CopyVoxel = CopyEvent.GetCurrentRLEVoxel();
			CurrentVoxel = CopyVoxel.Voxel;
			RemainingIndex = CopyVoxel.RunLenght;
		}
		else
		{
			RemainingIndex = LeadingEvent.GetEventIndex() - VoxelIndex;
			LeadingEvent.GetCurrentRLEVoxel().RunLenght -= RemainingIndex;
			CurrentVoxel = CopyEvent.GetCurrentVoxel();
		}

		const auto& EditVoxel = EditEvent.GetCurrentRLEVoxel();
		while (EditVoxel.RunLenght > RemainingIndex)
		{
			CopyEvent.AdvanceEvent();
			CopyVoxel = CopyEvent.GetCurrentRLEVoxel();
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
			LeadingEvent.VoxelGridPtr->Last().RunLenght += CopyEvent.GetCurrentRLEVoxel().RunLenght;
		}

		AdvanceEditInterval(VoxelEdits);
	}
	else
	{
		const int CopyEventIndex = CopyEvent.GetEventIndex();
		if (CopyEventIndex == VoxelIndex)
		{
			CopyEvent.AdvanceEvent();
			LeadingEvent.VoxelGridPtr->Add(CopyEvent.GetCurrentRLEVoxel());
		}
	}

	TryUpdateNextMeshingEvent(CopyEvent.GetEventIndex());
	TryUpdateNextMeshingEvent(EditEvent.LastEventIndex);
}

void FVoxelEventMesher::CreateBorder(FChunkBorderContext& BorderContext, const uint32 YEnd,
                                     const FRLEVoxel& CurrentVoxelSample,
                                     const EFaceDirection Direction, FIntVector BorderPosition,
                                     const bool BorderCondition)
{
	if (BorderCondition)
	{
		auto& CurrentVoxel = CurrentVoxelSample.Voxel;
		FIntVector CurrentVoxelPosition = VoxelPosition;

		// This for loop may be removed in the future if someone will implement interval checking in between borders
		for (uint32 y = 0; y < YEnd; y++)
		{
			if (BorderContext.CanGenerateBorder(Direction, BorderPosition, CurrentVoxel))
			{
				// If checking intervals is implemented, run may be larger than 1
				constexpr int RunLenght = 1;
				VirtualFaces[0].AddNewVirtualFace(Direction, CurrentVoxel,
				                                  CurrentVoxelPosition + VoxelPositionOffsets[Direction], RunLenght);
			}
			CurrentVoxelPosition.Y++;
			BorderPosition.Y++;
		}
	}
}

void FVoxelEventMesher::InternalReset()
{
	Reset();

	IndexSequenceBetweenEvents = 0;
	PreviousVoxelRun = nullptr;
	VoxelPosition = FIntVector(0, 0, 0);
	ContinueEditIndex = 0;
	EditEnabled = false;
	PreviousPosition = FIntVector(0, 0, 0);
}

void FVoxelEventMesher::AdvanceAllMeshingEvents()
{
	auto& LeadingMeshingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	if (AdvanceMeshingEvent(LeadingMeshingEvent))
	{
		const auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();

		// Left
		if ((PreviousVoxelRun->Voxel.IsEmptyVoxel() || PreviousVoxelRun->Voxel.IsTransparent()
			&& !LeadingMeshingEventVoxel.IsEmptyVoxel()) && VoxelPosition.Y != 0)
		{
			VirtualFaces[VoxelPosition.Y].AddNewVirtualFace(EFaceDirection::Left,
			                                                LeadingMeshingEventVoxel, VoxelPosition,
			                                                IndexSequenceBetweenEvents);
		}

		// Right
		const int VoxelPlaneIndex = PreviousPosition.Y + IndexSequenceBetweenEvents;
		if (!PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.IsEmptyVoxel() ||
			LeadingMeshingEventVoxel.IsTransparent()) && VoxelPlaneIndex != VoxelLine)
		{
			VirtualFaces[VoxelPlaneIndex].AddNewVirtualFace(EFaceDirection::Right,
			                                                PreviousVoxelRun->Voxel, PreviousPosition,
			                                                IndexSequenceBetweenEvents + 1);
		}
	}

	// Calculate index
	// Smallest interval should always be increase of Y dimension

	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingXInterval]);
	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingZInterval]);
}

void FVoxelEventMesher::ConvertVirtualFacesToMesh(FVoxelMesh& VoxelMeshContainer,
                                                  const double VoxelSize)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - Directional Greedy Merge");
#endif

	for (uint32 y = 0; y < VoxelLine; y++)
	{
		VirtualFaces[y].DirectionalGreedyMergeForVoxelPlane(&FirstDirectionalMeshingHelperArray,
		                                                    &SecondDirectionalMeshingHelperArray,
		                                                    VoxelMeshContainer, VoxelSize, MaxVoxelsInChunk);
	}
}
