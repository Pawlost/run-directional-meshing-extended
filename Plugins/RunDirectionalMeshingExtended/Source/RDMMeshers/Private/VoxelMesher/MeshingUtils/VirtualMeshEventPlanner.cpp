#include "VoxelMesher/MeshingUtils/VirtualMeshEventPlanner.h"
#include "VoxelMesher/VoxelMesherBase.h"

void VirtualMeshEventPlanner::AdvanceEditInterval(uint32 MaxNumberOfVoxel,
	TArray<FRLEVoxelEdit>& VoxelEdits)
{
	auto& EditEvent = MeshingEvents[EMeshingEventIndex::EditEvent];

	if (!VoxelEdits.IsEmpty())
	{
		auto VoxelEdit = VoxelEdits.Pop();
		EditEvent.LastEventIndex = VoxelEdit.EditEventIndex;
		// TODO: rewrite empty
		// must remain empty because only one edit can be active at time
		EditEvent.VoxelGridPtr->Empty();
		EditEvent.VoxelGridPtr->Push(VoxelEdit.EditVoxel);
	}
	else
	{
		constexpr int Offset = 1;
		EditEvent.LastEventIndex = MaxNumberOfVoxel + Offset;
	}
}

void VirtualMeshEventPlanner::TryUpdateNextMeshingEvent(const uint32 EventIndex)
{
	NextMeshingEventIndex = FMath::Min(EventIndex, NextMeshingEventIndex);
}

VirtualMeshEventPlanner::VirtualMeshEventPlanner(const uint32 VoxelLine,
	const uint32 VoxelPlane, const uint32 MaxNumberOfVoxels, 
	TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT>& VirtualFaces,
	TStaticArray<FVirtualVoxelFaceContainer, CHUNK_FACE_COUNT>& SideFaces, bool ShowBorders)
	: VoxelLine(VoxelLine), VoxelPlane(VoxelPlane), MaxNumberOfVoxels(MaxNumberOfVoxels), ShowBorders(ShowBorders)
{
	this->SideFaces = SideFaces;
	this->VirtualFaces = VirtualFaces;
}

void VirtualMeshEventPlanner::InitializeIntervals(TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid, 
	TArray<FRLEVoxelEdit>& VoxelEdits)
{
	TSharedPtr<TArray<FRLEVoxel>> MainVoxelGrid;
	// Set first run to trigger first condition in while loop
	if (!VoxelEdits.IsEmpty())
	{
		auto NewVoxelGrid = MakeShared<TArray<FRLEVoxel>>();
		constexpr int PreallocationEstimation = 3;
		NewVoxelGrid->Reserve(RLEVoxelGrid->Num() + VoxelEdits.Num() * PreallocationEstimation);
		EditEnabled = true;
    
		auto VoxelEdit = VoxelEdits.Pop();
		int CopyVoxelRunIndex = -1;
		TSharedPtr<TArray<FRLEVoxel>> EditEventArray = MakeShared<TArray<FRLEVoxel>>();
    
		EditEventArray->Push(VoxelEdit.EditVoxel);
		MeshingEvents[EMeshingEventIndex::EditEvent] = {EditEventArray, VoxelEdit.EditEventIndex, 0};
    
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
    
			AdvanceEditInterval(MaxNumberOfVoxels, VoxelEdits);
    
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
    
		MeshingEvents[EMeshingEventIndex::CopyEvent] = {RLEVoxelGrid, Offset, CopyVoxelRunIndex};
		MainVoxelGrid = NewVoxelGrid;
	}else
	{
		MainVoxelGrid = RLEVoxelGrid;
	}

	MeshingEvents[EMeshingEventIndex::LeadingInterval] = {MainVoxelGrid, 0, 0};
	MeshingEvents[EMeshingEventIndex::FollowingXInterval] = {MainVoxelGrid, VoxelPlane, 0};
	MeshingEvents[EMeshingEventIndex::FollowingZInterval] = {MainVoxelGrid, VoxelLine, 0};
}

void VirtualMeshEventPlanner::GenerateVirtualFaces()
{
	// Traverse through voxel grid
	while (CurrentMeshingEventIndex < MaxNumberOfVoxels)
	{
		// TODO: try to rewrite using substraction
		CurrentVoxelPosition = FIntVector<uint32>(
			CurrentMeshingEventIndex / (VoxelPlane), 
			((CurrentMeshingEventIndex / VoxelLine) % VoxelLine),
			CurrentMeshingEventIndex % VoxelLine);
		TraverseYDirection( SideMeshers, BorderIndexParams);
	}
}

void VirtualMeshEventPlanner::TraverseYDirection(TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
													   TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams)
{
	// Check borders
	do
	{
		// Reset interval flag
		NextMeshingEventIndex = MaxNumberOfVoxels;

		// Edit Interval
		if (EditEnabled)
		{
			EditVoxelGrid();
		}

		AdvanceAllMeshingEvents();

		CreateVirtualVoxelFacesInLShape(SideMeshers, BorderIndexParams);

		// Meshing event was finished
		CurrentMeshingEventIndex = NextMeshingEventIndex;
		CurrentVoxelPosition.Y += IndexSequenceBetweenEvents;

		PreviousVoxelRun = &MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
	}
	while (CurrentVoxelPosition.Y < VoxelLine);

	if (!PreviousVoxelRun->IsVoxelEmpty())
	{
		// Right Border
		CreateBorder(SideMeshers, BorderIndexParams, *PreviousVoxelRun, 1, EFaceDirection::Right, CurrentVoxelPosition.X, 0, CurrentVoxelPosition.Z, true);
	}
}


void VirtualMeshEventPlanner::CreateVirtualVoxelFacesInLShape(TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
                                                                    TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams)
{
	auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	auto& FollowingXEvent = MeshingEvents[EMeshingEventIndex::FollowingXInterval];
	auto& FollowingZEvent = MeshingEvents[EMeshingEventIndex::FollowingZInterval];

	// Sample interval
	// Move interval one step ahead if at the run end
	bool IsLeadingEmpty = LeadingEvent.GetCurrentVoxel().IsVoxelEmpty();
	bool IsFollowingXEmpty = FollowingXEvent.GetCurrentVoxel().IsVoxelEmpty();
	bool IsFollowingZEmpty = FollowingZEvent.GetCurrentVoxel().IsVoxelEmpty();

	// Generate culled faces
	if (!IsLeadingEmpty || !IsFollowingXEmpty || !IsFollowingZEmpty)
	{
		bool IsLeadingTransparent = LeadingEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingXTransparent = FollowingXEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingZTransparent = FollowingZEvent.GetCurrentVoxel().IsTransparent();

		const uint32 MaxYSequence = (VoxelLine - CurrentVoxelPosition.Y) + CurrentMeshingEventIndex;
		TryUpdateNextMeshingEvent(MaxYSequence);
		IndexSequenceBetweenEvents = NextMeshingEventIndex - CurrentMeshingEventIndex;

		auto& LeadingEventVoxel = LeadingEvent.GetCurrentVoxel();
		if (!IsLeadingEmpty)
		{
			// Left border
			CreateBorder(SideMeshers, BorderIndexParams, CurrentVoxelPosition, LeadingEventVoxel, 1,
			             EFaceDirection::Left, X, VoxelLine - 1, Z, CurrentVoxelPosition.Y == 0);

			// Front border
			CreateBorder(SideMeshers, BorderIndexParams, X + 1, Y, Z, LeadingEventVoxel, EFaceDirection::Front, 
				0, Y, Z, CurrentVoxelPosition.X == VoxelLine - 1);

			// Top border
			CreateBorder( SideMeshers, BorderIndexParams, X, Y, Z + 1, LeadingEventVoxel,
			             EFaceDirection::Top, X, Y, 0, CurrentVoxelPosition.Z == VoxelLine - 1);

			// Bottom border
			CreateBorder(SideMeshers, BorderIndexParams, CurrentVoxelPosition, LeadingEventVoxel,
			              EFaceDirection::Bottom, X, Y, VoxelLine - 1, CurrentVoxelPosition.Z == 0);

			//Back borders
			CreateBorder(SideMeshers, BorderIndexParams, CurrentVoxelPosition, LeadingEventVoxel,
			            EFaceDirection::Back, VoxelLine - 1, Y, Z, CurrentVoxelPosition.X == 0);
		}

		PreviousPosition = CurrentVoxelPosition;

		CreateFace(VirtualFaces, FStaticMergeData::FrontFaceData,
		           FollowingXEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			           IsFollowingXTransparent)));

		CreateFace(VirtualFaces, FStaticMergeData::TopFaceData, 
		           FollowingZEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			           IsFollowingZTransparent)));

		CreateFace(VirtualFaces, FStaticMergeData::BackFaceData,
		           LeadingEventVoxel, IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
			           IsFollowingXTransparent)));

		CreateFace(VirtualFaces, FStaticMergeData::BottomFaceData,
		           LeadingEventVoxel, IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
			           IsFollowingZTransparent)));
	}
	else
	{
		// Skip large empty sequences
		IndexSequenceBetweenEvents = NextMeshingEventIndex - CurrentMeshingEventIndex;
	}
}

void VirtualMeshEventPlanner::CreateFace(
	const TStaticArray<TArray<FVirtualVoxelFaceContainer>, CHUNK_FACE_COUNT>& VirtualFaces,
	const int FaceIndex, const FRLEVoxel& RLEVoxel, const int YEnd, const bool CanGenerate, int VoxelPlanePosition)
{
	if (CanGenerate)
	{
		VirtualFaces[FaceIndex][VoxelPlanePosition].AddFace(FaceIndex, RLEVoxel.Voxel, CurrentVoxelPosition, YEnd);
	}
}

bool VirtualMeshEventPlanner::AdvanceMeshingEvent(const EMeshingEventIndex IntervalFlagIndex)
{
	bool AdvanceInterval = false;
	auto& Interval = MeshingEvents[IntervalFlagIndex];
	int IntervalEventIndex = Interval.GetEventIndex();
	if (IntervalEventIndex == CurrentMeshingEventIndex)
	{
		// Advance interval in chunk voxel sequence
		Interval.AdvanceEvent();
		AdvanceInterval = true;
	}

	IntervalEventIndex = Interval.GetEventIndex();
	TryUpdateNextMeshingEvent(IntervalEventIndex);

	return AdvanceInterval;
}

void VirtualMeshEventPlanner::EditVoxelGrid()
{
	auto& CopyEvent = MeshingEvents[EMeshingEventIndex::CopyEvent];
	auto& EditEvent = MeshingEvents[EMeshingEventIndex::EditEvent];
	auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];

	if (EditEvent.LastEventIndex == CurrentMeshingEventIndex)
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
			RemainingIndex = LeadingEvent.GetEventIndex() - CurrentMeshingEventIndex;
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

		AdvanceEditInterval();
	}
	else
	{
		int CopyEventIndex = CopyEvent.GetEventIndex();
		if (CopyEventIndex == CurrentMeshingEventIndex)
		{
			CopyEvent.AdvanceEvent();
			LeadingEvent.VoxelGridPtr->Add(CopyEvent.GetCurrentVoxel());
		}
	}

	TryUpdateNextMeshingEvent(CopyEvent.GetEventIndex());
	TryUpdateNextMeshingEvent(EditEvent.LastEventIndex);
}

void VirtualMeshEventPlanner::CreateBorder(
	TStaticArray<TStrongObjectPtr<UVoxelMesherBase>, CHUNK_FACE_COUNT>& SideMeshers,
	TStaticArray<VirtualMeshEventPlanner, CHUNK_FACE_COUNT>& BorderIndexParams,
	FIntVector VoxelPosition,
	 const FRLEVoxel& CurrentVoxelSample,
	EFaceDirection Direction,
	FIntVector SideChunkBorderPosition, bool BorderCondition)
{
	if (BorderCondition)
	{
		auto Mesher = SideMeshers[Direction];
		
		bool CanGenerate = ShowBorders && Mesher == nullptr;
		auto& CurrentVoxel = CurrentVoxelSample.Voxel;
		
		// This for loop may be removed in the future if someone will implement interval checking in between borders
		for (int y = 0; y < IndexSequenceBetweenEvents; y++)
		{
			if (!CanGenerate && Mesher != nullptr) 
			{
				SideChunkBorderPosition.Y++;
				auto BorderVoxel = Mesher->GetBorderVoxel(BorderIndexParams[Direction], SideChunkBorderPosition);
				CanGenerate = BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.IsTransparent());
			}
				
			if (CanGenerate)
			{
				// If checking intervals is implement run may be larger than 1
				constexpr int RunLenght = 1;
				VoxelPosition.Y ++;
				SideFaces[Direction].AddFace(Direction, CurrentVoxel, VoxelPosition, RunLenght);
			}
		}
	}
}


void VirtualMeshEventPlanner::AdvanceAllMeshingEvents()
{
	if (AdvanceMeshingEvent(EMeshingEventIndex::LeadingInterval))
	{
		const auto& LeadingMeshingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
		auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();

		// Left
		if ((PreviousVoxelRun->Voxel.IsEmptyVoxel() || PreviousVoxelRun->Voxel.IsTransparent()
			&& !LeadingMeshingEventVoxel.IsVoxelEmpty()) && CurrentVoxelPosition.Y != 0)
		{
		// TODO rewrite voxel plane index
			VirtualFaces[EFaceDirection::Left][0].CreateSideFace(EFaceDirection::Left,
						   CurrentVoxelPosition,
						   LeadingMeshingEventVoxel.Voxel, IndexSequenceBetweenEvents);
		}

		// Right
		if (!PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.Voxel.IsEmptyVoxel() ||
				LeadingMeshingEventVoxel.Voxel.IsTransparent()) && PreviousPosition.Y + IndexSequenceBetweenEvents != VoxelLine) 
		{
			VirtualFaces[EFaceDirection::Right][0].CreateSideFace(EFaceDirection::Right,
						   PreviousPosition, PreviousVoxelRun->Voxel, IndexSequenceBetweenEvents + 1);
		}
	}

	// Calculate index
	// Smallest interval should always be increase of Y dimension

	AdvanceMeshingEvent(EMeshingEventIndex::FollowingXInterval);
	AdvanceMeshingEvent(EMeshingEventIndex::FollowingZInterval);
}

