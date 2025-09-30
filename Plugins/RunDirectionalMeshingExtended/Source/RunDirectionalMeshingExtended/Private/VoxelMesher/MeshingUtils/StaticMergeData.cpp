#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	true,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateFrontFace,
	FVoxelFace::FrontBorderLocation,
	FVoxelFace::FaceRowConditionZ
);

FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	false,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateBackFace,
	FVoxelFace::BackBorderLocation,
	FVoxelFace::FaceRowConditionZ
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	true,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateRightFace,
	FVoxelFace::RightBorderLocation,
	FVoxelFace::FaceRowConditionZ
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	false,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateLeftFace,
	FVoxelFace::LeftBorderLocation,
	FVoxelFace::FaceRowConditionZ
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	true,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateTopFace,
	FVoxelFace::TopBorderLocation,
	FVoxelFace::FaceRowConditionX
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	false,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::MergeFaceUp,
	FVoxelFace::CreateBottomFace,
	FVoxelFace::BottomBorderLocation,
	FVoxelFace::FaceRowConditionX
);