#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateFrontFace,
	FVirtualVoxelFace::FrontBorderLocation,
	FVirtualVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBackFace,
	FVirtualVoxelFace::BackBorderLocation,
	FVirtualVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::CreateRightFace,
	FVirtualVoxelFace::RightBorderLocation,
	FVirtualVoxelFace::MergeFailConditionRightY
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::CreateLeftFace,
	FVirtualVoxelFace::LeftBorderLocation,
	FVirtualVoxelFace::MergeFailConditionLeftY
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateTopFace,
	FVirtualVoxelFace::TopBorderLocation,
	FVirtualVoxelFace::MergeFailConditionZ
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBottomFace,
	FVirtualVoxelFace::BottomBorderLocation,
	FVirtualVoxelFace::MergeFailConditionZ
);