#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	true,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateFrontFace,
	FVirtualVoxelFace::FrontBorderLocation,
	FVirtualVoxelFace::GetXLocation,
	FVirtualVoxelFace::HeightConditionZ,
	FVirtualVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	false,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBackFace,
	FVirtualVoxelFace::BackBorderLocation,
	FVirtualVoxelFace::GetXLocation,
	FVirtualVoxelFace::HeightConditionZ,
	FVirtualVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	true,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::CreateRightFace,
	FVirtualVoxelFace::RightBorderLocation,
	FVirtualVoxelFace::GetYLocation,
	FVirtualVoxelFace::HeightConditionX,
	FVirtualVoxelFace::MergeFailConditionRightY
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	false,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::CreateLeftFace,
	FVirtualVoxelFace::LeftBorderLocation,
	FVirtualVoxelFace::GetYLocation,
	FVirtualVoxelFace::HeightConditionX,
	FVirtualVoxelFace::MergeFailConditionLeftY
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	true,
	FVirtualVoxelFace::MergeFaceEnd,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateTopFace,
	FVirtualVoxelFace::TopBorderLocation,
	FVirtualVoxelFace::GetZLocation,
	FVirtualVoxelFace::HeightConditionX,
	FVirtualVoxelFace::MergeFailConditionZ
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	false,
	FVirtualVoxelFace::MergeFaceStart,
	FVirtualVoxelFace::MergeFaceUp,
	FVirtualVoxelFace::CreateBottomFace,
	FVirtualVoxelFace::BottomBorderLocation,
	FVirtualVoxelFace::GetZLocation,
	FVirtualVoxelFace::HeightConditionX,
	FVirtualVoxelFace::MergeFailConditionZ
);