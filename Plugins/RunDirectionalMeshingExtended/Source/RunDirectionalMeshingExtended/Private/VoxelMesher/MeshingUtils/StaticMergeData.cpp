#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	EFaceDirection::Back,
	true,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateFrontFace,
	FVoxelFace::FrontBorderLocation
);


FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	EFaceDirection::Front,
	false,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateBackFace,
	FVoxelFace::BackBorderLocation
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	EFaceDirection::Left,
	true,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateRightFace,
	FVoxelFace::RightBorderLocation
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	EFaceDirection::Right,
	false,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateLeftFace,
	FVoxelFace::LeftBorderLocation
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	EFaceDirection::Bottom,
	true,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateTopFace,
	FVoxelFace::TopBorderLocation
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	EFaceDirection::Top,
	false,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateBottomFace,
	FVoxelFace::BottomBorderLocation
);

//-------------------


FStaticGreedyMergeData FStaticGreedyMergeData:: FrontFace = FStaticGreedyMergeData(
	EFaceDirection::Front,
	FVoxelFace::FaceRowConditionZ
);

FStaticGreedyMergeData FStaticGreedyMergeData::BackFace = FStaticGreedyMergeData(
	EFaceDirection::Back,
	FVoxelFace::FaceRowConditionZ
);

FStaticGreedyMergeData FStaticGreedyMergeData::RightFace = FStaticGreedyMergeData(
	EFaceDirection::Right,
	FVoxelFace::FaceRowConditionZ
);

FStaticGreedyMergeData FStaticGreedyMergeData::LeftFace = FStaticGreedyMergeData(
	EFaceDirection::Left,
	FVoxelFace::FaceRowConditionZ
);

FStaticGreedyMergeData FStaticGreedyMergeData::TopFace = FStaticGreedyMergeData(
	EFaceDirection::Top,
	FVoxelFace::FaceRowConditionX
);

FStaticGreedyMergeData FStaticGreedyMergeData::BottomFace = FStaticGreedyMergeData(
	EFaceDirection::Bottom,
	FVoxelFace::FaceRowConditionX
);
