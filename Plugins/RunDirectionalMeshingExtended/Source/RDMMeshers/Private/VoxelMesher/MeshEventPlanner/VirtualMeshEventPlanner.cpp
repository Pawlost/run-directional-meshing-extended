#include "VoxelMesher/MeshEventPlanner/VirtualMeshEventPlanner.h"
#include "VoxelMesher/VirtualChunk.h"
#include "VoxelMesher/MeshingUtils/VirtualVoxelFaceContainer.h"

void FVirtualMeshEventPlanner::AdvanceEditInterval(TArray<FRLEVoxelEdit>& VoxelEdits)
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

FVirtualMeshEventPlanner::FVirtualMeshEventPlanner(const uint32 VoxelLine,
                                                   const uint32 VoxelPlane, const uint32 MaxNumberOfVoxels)
	: FVirtualMeshEventPlannerBase(VoxelLine, VoxelPlane, MaxNumberOfVoxels)
{
	for (uint32 y = 0; y < VoxelLine; y++)
	{
		VirtualFaces.Emplace(VoxelPlane);
	}
}

void FVirtualMeshEventPlanner::UpdateInternalState(const EBorderVisualizationOption BorderVisualizationParam,
                                                   const uint32 VoxelLineParam, const uint32 VoxelPlaneParam,
                                                   const uint32 MaxVoxelsInChunkParam)
{
	BorderVisualization = BorderVisualizationParam;
	VoxelLine = VoxelLineParam;
	VoxelPlane = VoxelPlaneParam;
	MaxVoxelsInChunk = MaxVoxelsInChunkParam;

	InternalReset();
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
	}
	else
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
	while (VoxelIndex < MaxVoxelsInChunk)
	{
		UpdatePositionFromIndex();

		TraverseYDirection(BorderParameters, VoxelEdits);
	}
}

void FVirtualMeshEventPlanner::TraverseYDirection(FBorderParams& BorderParameters, TArray<FRLEVoxelEdit>& VoxelEdits)
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
		// Right Border
		CreateBorder(BorderParameters, VoxelPosition, 1, *PreviousVoxelRun, EFaceDirection::Right,
		             FIntVector(VoxelPosition.X, 0, VoxelPosition.Z), true);
	}
}


void FVirtualMeshEventPlanner::CreateVirtualVoxelFacesInLShape(FBorderParams& BorderParameters)
{
	auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	auto& FollowingXEvent = MeshingEvents[EMeshingEventIndex::FollowingXInterval];
	auto& FollowingZEvent = MeshingEvents[EMeshingEventIndex::FollowingZInterval];

	// Sample interval
	// Move interval one step ahead if at the run end
	bool IsLeadingEmpty = LeadingEvent.GetCurrentRLEVoxel().IsVoxelEmpty();
	bool IsFollowingXEmpty = FollowingXEvent.GetCurrentRLEVoxel().IsVoxelEmpty();
	bool IsFollowingZEmpty = FollowingZEvent.GetCurrentRLEVoxel().IsVoxelEmpty();

	// Generate culled faces
	if (!IsLeadingEmpty || !IsFollowingXEmpty || !IsFollowingZEmpty)
	{
		bool IsLeadingTransparent = LeadingEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingXTransparent = FollowingXEvent.GetCurrentVoxel().IsTransparent();
		bool IsFollowingZTransparent = FollowingZEvent.GetCurrentVoxel().IsTransparent();

		const uint32 MaxYSequence = (VoxelLine - VoxelPosition.Y) + VoxelIndex;
		TryUpdateNextMeshingEvent(MaxYSequence);
		IndexSequenceBetweenEvents = NextVoxelIndex - VoxelIndex;

		auto& LeadingEventVoxel = LeadingEvent.GetCurrentRLEVoxel();
		if (!IsLeadingEmpty)
		{
			// Left border
			FIntVector BorderVoxelPosition = VoxelPosition;;
			BorderVoxelPosition.Y = VoxelLine - 1;

			CreateBorder(BorderParameters, VoxelPosition, 1, LeadingEventVoxel,
			             EFaceDirection::Left, BorderVoxelPosition, VoxelPosition.Y == 0);

			// Front border
			BorderVoxelPosition = VoxelPosition;;
			BorderVoxelPosition.X = 0;
			CreateBorder(BorderParameters, VoxelPosition + FIntVector(1, 0, 0), IndexSequenceBetweenEvents,
			             LeadingEventVoxel, EFaceDirection::Front,
			             BorderVoxelPosition, VoxelPosition.X == VoxelLine - 1);

			// Top border
			BorderVoxelPosition = VoxelPosition;
			BorderVoxelPosition.Z = 0;
			CreateBorder(BorderParameters, VoxelPosition + FIntVector(0, 0, 1), IndexSequenceBetweenEvents,
			             LeadingEventVoxel, EFaceDirection::Top,
			             BorderVoxelPosition, VoxelPosition.Z + 1 == VoxelLine);

			// Bottom border
			BorderVoxelPosition = VoxelPosition;
			BorderVoxelPosition.Z = VoxelLine - 1;
			CreateBorder(BorderParameters, VoxelPosition, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Bottom, BorderVoxelPosition, VoxelPosition.Z == 0);

			//Back borders
			BorderVoxelPosition = VoxelPosition;
			BorderVoxelPosition.X = VoxelLine - 1;
			CreateBorder(BorderParameters, VoxelPosition, IndexSequenceBetweenEvents, LeadingEventVoxel,
			             EFaceDirection::Back, BorderVoxelPosition, VoxelPosition.X == 0);
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

void FVirtualMeshEventPlanner::EditVoxelGrid(TArray<FRLEVoxelEdit>& VoxelEdits)
{
	auto& CopyEvent = MeshingEvents[EMeshingEventIndex::CopyEvent];
	auto& EditEvent = MeshingEvents[EMeshingEventIndex::EditEvent];
	auto& LeadingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];

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

		auto& EditVoxel = EditEvent.GetCurrentRLEVoxel();
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
		int CopyEventIndex = CopyEvent.GetEventIndex();
		if (CopyEventIndex == VoxelIndex)
		{
			CopyEvent.AdvanceEvent();
			LeadingEvent.VoxelGridPtr->Add(CopyEvent.GetCurrentRLEVoxel());
		}
	}

	TryUpdateNextMeshingEvent(CopyEvent.GetEventIndex());
	TryUpdateNextMeshingEvent(EditEvent.LastEventIndex);
}

void FVirtualMeshEventPlanner::CreateBorder(FBorderParams& BorderParameters,
                                            FIntVector CurrentVoxelPosition, uint32 YEnd,
                                            const FRLEVoxel& CurrentVoxelSample,
                                            EFaceDirection Direction,
                                            FIntVector SideChunkBorderPosition, bool BorderCondition)
{
	if (BorderCondition)
	{
		auto Mesher = BorderParameters.SideMeshers[Direction];

		auto& CurrentVoxel = CurrentVoxelSample.Voxel;

		// This for loop may be removed in the future if someone will implement interval checking in between borders
		for (uint32 y = 0; y < YEnd; y++)
		{
			bool CanGenerate = BorderVisualization == EBorderVisualizationOption::All || BorderVisualization ==
				EBorderVisualizationOption::OnlyOuterBorders && Mesher == nullptr;

			if (!CanGenerate && Mesher != nullptr)
			{
				auto BorderVoxel = Mesher->GetBorderVoxel(BorderParameters.BorderIndexParams[Direction],
				                                          SideChunkBorderPosition);

				CanGenerate = BorderVoxel.IsEmptyVoxel() || (BorderVoxel.IsTransparent() && !CurrentVoxel.
					IsTransparent());
			}
			SideChunkBorderPosition.Y++;

			if (CanGenerate)
			{
				// If checking intervals is implement run may be larger than 1
				constexpr int RunLenght = 1;
				VirtualFaces[0].AddNewVirtualFace(Direction, CurrentVoxel, CurrentVoxelPosition, RunLenght);
			}
			CurrentVoxelPosition.Y++;
		}
	}
}

void FVirtualMeshEventPlanner::InternalReset()
{
	Reset();

	IndexSequenceBetweenEvents = 0;
	PreviousVoxelRun = nullptr;
	VoxelPosition = FIntVector(0, 0, 0);
	ContinueEditIndex = 0;
	EditEnabled = false;
	PreviousPosition = FIntVector(0, 0, 0);
}

void FVirtualMeshEventPlanner::AdvanceAllMeshingEvents()
{
	auto& LeadingMeshingEvent = MeshingEvents[EMeshingEventIndex::LeadingInterval];
	if (AdvanceMeshingEvent(LeadingMeshingEvent))
	{
		auto& LeadingMeshingEventVoxel = LeadingMeshingEvent.GetCurrentVoxel();

		// Left
		if ((PreviousVoxelRun->Voxel.IsEmptyVoxel() || PreviousVoxelRun->Voxel.IsTransparent()
			&& !LeadingMeshingEventVoxel.IsEmptyVoxel()) && VoxelPosition.Y != 0)
		{
			VirtualFaces[VoxelPosition.Y].AddNewVirtualFace(EFaceDirection::Left,
				LeadingMeshingEventVoxel, VoxelPosition, IndexSequenceBetweenEvents);
		}

		// Right
		int VoxelPlaneIndex = PreviousPosition.Y + IndexSequenceBetweenEvents;
		if (!PreviousVoxelRun->IsVoxelEmpty() && (LeadingMeshingEventVoxel.IsEmptyVoxel() ||
			LeadingMeshingEventVoxel.IsTransparent()) && VoxelPlaneIndex != VoxelLine)
		{
			VirtualFaces[VoxelPlaneIndex].AddNewVirtualFace(EFaceDirection::Right,
				PreviousVoxelRun->Voxel, PreviousPosition, IndexSequenceBetweenEvents + 1);
		}
	}

	// Calculate index
	// Smallest interval should always be increase of Y dimension

	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingXInterval]);
	AdvanceMeshingEvent(MeshingEvents[EMeshingEventIndex::FollowingZInterval]);
}

void FVirtualMeshEventPlanner::ConvertVirtualFacesToMesh(FVoxelMeshContainer& VoxelMeshContainer,
                                                      const double VoxelSize)
{
#if CPUPROFILERTRACE_ENABLED
	TRACE_CPUPROFILER_EVENT_SCOPE("Meshing - Directional Greedy Merge");
#endif

	for (int f = 0; f < VOXEL_FACE_COUNT; f++)
	{
		for (uint32 y = 0; y < VoxelLine; y++)
		{
			VirtualFaces[y].DirectionalGreedyMergeForVoxelPlane(FirstDirectionalMeshingHelperArray,
			                                                       SecondDirectionalMeshingHelperArray,
			                                                       VoxelMeshContainer, static_cast<EFaceDirection>(f),
			                                                       VoxelSize, MaxVoxelsInChunk);
		}
	}
}
