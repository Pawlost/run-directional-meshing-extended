#include "VoxelMesher/MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "VoxelMesher/VoxelMesherBase.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFaceContainer.h"

void FVirtualMeshEventPlanner::AdvanceEditInterval(TArray<FRLEVoxelEdit>& VoxelEdits)
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
		EditEvent.LastEventIndex = MaxVoxelsInChunk + Offset;
	}
}

FVirtualMeshEventPlanner::FVirtualMeshEventPlanner(const uint32 VoxelLine,
	const uint32 VoxelPlane, const uint32 MaxNumberOfVoxels, bool ShowBorders)
	: ShowBorders(ShowBorders), VoxelLine(VoxelLine), VoxelPlane(VoxelPlane), MaxVoxelsInChunk(MaxNumberOfVoxels)
{
	constexpr int EstimatedRows = 3;
		
	FirstArray.Reserve(VoxelLine * EstimatedRows);
	SecondArray.Reserve(VoxelLine * EstimatedRows);
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < VoxelLine; y++)
		{
			VirtualFaces[f].Emplace(VoxelPlane);
		}
	}
}

void FVirtualMeshEventPlanner::InitializeIntervals(TSharedPtr<TArray<FRLEVoxel>>& RLEVoxelGrid, 
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
    
			AdvanceEditInterval(VoxelEdits);
    
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

void FVirtualMeshEventPlanner::GenerateVirtualFaces(FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
{
	#if CPUPROFILERTRACE_ENABLED
    	TRACE_CPUPROFILER_EVENT_SCOPE("RLE Meshing - RunDirectionalMeshing from RLECompression generation")
    #endif
	
	// Traverse through voxel grid
	while (CurrentMeshingEventIndex < MaxVoxelsInChunk)
	{
		// TODO: try to rewrite using substraction
		CurrentVoxelPosition = FIntVector(
			CurrentMeshingEventIndex / (VoxelPlane), 
			CurrentMeshingEventIndex % VoxelLine ,
			((CurrentMeshingEventIndex / VoxelLine) % VoxelLine));
		TraverseYDirection( BorderParameters, VoxelEdits);
	}
}

void FVirtualMeshEventPlanner::TraverseYDirection(FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
{
	// Check borders
	do
	{
		// Reset interval flag
		NextMeshingEventIndex = MaxVoxelsInChunk;

		// Edit Interval
		if (EditEnabled)
		{
			EditVoxelGrid(VoxelEdits);
		}

		AdvanceAllMeshingEvents();

		CreateVirtualVoxelFacesInLShape(BorderParameters);

		// Meshing event was finished
		CurrentMeshingEventIndex = NextMeshingEventIndex;
		CurrentVoxelPosition.Y += IndexSequenceBetweenEvents;

		PreviousVoxelRun = &MeshingEvents[EMeshingEventIndex::LeadingInterval].GetCurrentVoxel();
	}
	while (static_cast<uint32>(CurrentVoxelPosition.Y) < VoxelLine);

	if (!PreviousVoxelRun->IsVoxelEmpty())
	{
		// Right Border
		CreateBorder(BorderParameters, CurrentVoxelPosition,1, *PreviousVoxelRun, EFaceDirection::Right, FIntVector(CurrentVoxelPosition.X, 0, CurrentVoxelPosition.Z), true);
	}
}


void FVirtualMeshEventPlanner::CreateVirtualVoxelFacesInLShape(FBorderParams& BorderParameters)
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
			FIntVector BorderVoxelPosition = CurrentVoxelPosition;;
			BorderVoxelPosition.Y = VoxelLine - 1;
			
			CreateBorder(BorderParameters, CurrentVoxelPosition, 1, LeadingEventVoxel, 
			             EFaceDirection::Left, BorderVoxelPosition, CurrentVoxelPosition.Y == 0);

			// Front border
			BorderVoxelPosition = CurrentVoxelPosition;;
			BorderVoxelPosition.X = 0;
			CreateBorder(BorderParameters, CurrentVoxelPosition + FIntVector(1,0,0), IndexSequenceBetweenEvents, LeadingEventVoxel, EFaceDirection::Front, 
				BorderVoxelPosition, CurrentVoxelPosition.X == VoxelLine - 1);

			// Top border
			BorderVoxelPosition = CurrentVoxelPosition;;
			BorderVoxelPosition.Z = 0;
			CreateBorder( BorderParameters, CurrentVoxelPosition + FIntVector(0,0,1), IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Top, BorderVoxelPosition, CurrentVoxelPosition.Z == VoxelLine - 1);

			// Bottom border
			BorderVoxelPosition = CurrentVoxelPosition;;
			BorderVoxelPosition.Z =  VoxelLine - 1;
			CreateBorder(BorderParameters, CurrentVoxelPosition, IndexSequenceBetweenEvents, LeadingEventVoxel, 
			              EFaceDirection::Bottom, BorderVoxelPosition, CurrentVoxelPosition.Z == 0);

			//Back borders
			BorderVoxelPosition = CurrentVoxelPosition;;
			BorderVoxelPosition.X =  VoxelLine - 1;
			CreateBorder(BorderParameters, CurrentVoxelPosition, IndexSequenceBetweenEvents, LeadingEventVoxel,
			            EFaceDirection::Back, BorderVoxelPosition, CurrentVoxelPosition.X == 0);
			
		}

		PreviousPosition = CurrentVoxelPosition;

		CreateFace(EFaceDirection::Front,
		           FollowingXEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.X != 0 && !IsFollowingXEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			           IsFollowingXTransparent)), CurrentVoxelPosition.X);

		CreateFace(EFaceDirection::Top,
		           FollowingZEvent.GetCurrentVoxel(), IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.Z != 0 && !IsFollowingZEmpty && (IsLeadingEmpty || (IsLeadingTransparent && !
			           IsFollowingZTransparent)), CurrentVoxelPosition.Z);

		CreateFace( EFaceDirection::Back,
		           LeadingEventVoxel, IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.X != 0 && !IsLeadingEmpty && (IsFollowingXEmpty || (!IsLeadingTransparent &&
			           IsFollowingXTransparent)), CurrentVoxelPosition.X);

		CreateFace(EFaceDirection::Bottom,
		           LeadingEventVoxel, IndexSequenceBetweenEvents,
		           CurrentVoxelPosition.Z != 0 && !IsLeadingEmpty && (IsFollowingZEmpty || (!IsLeadingTransparent &&
			           IsFollowingZTransparent)), CurrentVoxelPosition.Z);
	}
	else
	{
		// Skip large empty sequences
		IndexSequenceBetweenEvents = NextMeshingEventIndex - CurrentMeshingEventIndex;
	}
}

void FVirtualMeshEventPlanner::CreateFace(
	const EFaceDirection FaceIndex, const FRLEVoxel& RLEVoxel, const int YEnd, const bool CanGenerate, int VoxelPlanePosition)
{
	if (CanGenerate)
	{
		VirtualFaces[FaceIndex][VoxelPlanePosition].AddFace(FaceIndex, RLEVoxel.Voxel, CurrentVoxelPosition, YEnd);
	}
}

void FVirtualMeshEventPlanner::EditVoxelGrid(TArray<FRLEVoxelEdit>& VoxelEdits)
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

		AdvanceEditInterval(VoxelEdits);
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

void FVirtualMeshEventPlanner::CreateBorder(FBorderParams& BorderParameters,
	FIntVector VoxelPosition, uint32 YEnd,
	 const FRLEVoxel& CurrentVoxelSample,
	EFaceDirection Direction,
	FIntVector SideChunkBorderPosition, bool BorderCondition)
{
	if (BorderCondition)
	{
		auto Mesher = BorderParameters.SideMeshers[Direction];
		
		bool CanGenerate = ShowBorders && Mesher == nullptr;
		auto& CurrentVoxel = CurrentVoxelSample.Voxel;
		
		// This for loop may be removed in the future if someone will implement interval checking in between borders
		for (uint32 y = 0; y < YEnd; y++)
		{
			if (!CanGenerate && Mesher != nullptr) 
			{
				auto BorderVoxel = Mesher->GetBorderVoxel(BorderParameters.BorderIndexParams[Direction], SideChunkBorderPosition);

				CanGenerate = BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.IsTransparent());
			}
			SideChunkBorderPosition.Y++;
			
			if (CanGenerate)
			{
				// If checking intervals is implement run may be larger than 1
				constexpr int RunLenght = 1;
				SideFaces[Direction].AddFace(Direction, CurrentVoxel, VoxelPosition, RunLenght);
			}
			VoxelPosition.Y += 1;
			
		}
	}
}

void FVirtualMeshEventPlanner::AdvanceAllMeshingEvents()
{
	auto& LeadingMeshingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	if (AdvanceMeshingEvent(LeadingMeshingEvent))
	{
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

	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingXInterval]);
	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingZInterval]);
}

void FVirtualMeshEventPlanner::DirectionalGreedyMerge(FVoxelMeshContainer& VoxelMeshContainer,
	const double VoxelSize)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - Directional Greedy Merge");
#endif
	
	for (int f = 0; f < CHUNK_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < VoxelLine; y++)
		{
			VirtualFaces[f][y].DirectionalGreedyMergeForVoxelPlane(FirstArray, SecondArray, VoxelMeshContainer, static_cast<EFaceDirection>(f), VoxelSize, MaxVoxelsInChunk);
		}

		SideFaces[f].DirectionalGreedyMergeForVoxelPlane(FirstArray, SecondArray, VoxelMeshContainer, static_cast<EFaceDirection>(f), VoxelSize, MaxVoxelsInChunk);
	}
}