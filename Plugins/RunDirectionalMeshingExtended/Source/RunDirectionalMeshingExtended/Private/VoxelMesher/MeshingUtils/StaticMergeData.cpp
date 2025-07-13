#include "VoxelMesher/MeshingUtils/StaticMergeData.h"

FStaticMergeData FStaticMergeData:: FrontFaceData = FStaticMergeData(
	EFaceDirection::Front,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateFrontFace
);


FStaticMergeData FStaticMergeData::BackFaceData = FStaticMergeData(
	EFaceDirection::Back,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateBackFace
);

FStaticMergeData FStaticMergeData::RightFaceData = FStaticMergeData(
	EFaceDirection::Right,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateRightFace
);

FStaticMergeData FStaticMergeData::LeftFaceData = FStaticMergeData(
	EFaceDirection::Left,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateLeftFace
);

FStaticMergeData FStaticMergeData::TopFaceData = FStaticMergeData(
	EFaceDirection::Top,
	FVoxelFace::MergeFaceEnd,
	FVoxelFace::CreateTopFace
);

FStaticMergeData FStaticMergeData::BottomFaceData = FStaticMergeData(
	EFaceDirection::Bottom,
	FVoxelFace::MergeFaceStart,
	FVoxelFace::CreateBottomFace
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
