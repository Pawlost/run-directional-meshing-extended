#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	true,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateFrontFace,
	FVoxelFace::FrontBorderLocation,
	FVoxelFace::GetXLocation,
	FVoxelFace::HeightConditionZ,
	FVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	false,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateBackFace,
	FVoxelFace::BackBorderLocation,
	FVoxelFace::GetXLocation,
	FVoxelFace::HeightConditionZ,
	FVoxelFace::MergeFailConditionX
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	true,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateRightFace,
	FVoxelFace::RightBorderLocation,
	FVoxelFace::GetYLocation,
	FVoxelFace::HeightConditionX,
	FVoxelFace::MergeFailConditionRightY
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	false,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateLeftFace,
	FVoxelFace::LeftBorderLocation,
	FVoxelFace::GetYLocation,
	FVoxelFace::HeightConditionX,
	FVoxelFace::MergeFailConditionLeftY
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	true,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateTopFace,
	FVoxelFace::TopBorderLocation,
	FVoxelFace::GetZLocation,
	FVoxelFace::HeightConditionX,
	FVoxelFace::MergeFailConditionZ
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	false,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateBottomFace,
	FVoxelFace::BottomBorderLocation,
	FVoxelFace::GetZLocation,
	FVoxelFace::HeightConditionX,
	FVoxelFace::MergeFailConditionZ
);