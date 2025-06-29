#include "VoxelMesher/MeshingUtils/FaceDirection.h"

FFaceToDirection FFaceToDirection::TopDirection = FFaceToDirection(EFaceDirection::Top, FIntVector(0, 0, 1));
FFaceToDirection FFaceToDirection::BottomDirection = FFaceToDirection(EFaceDirection::Bottom, FIntVector(0, 0, -1));
FFaceToDirection FFaceToDirection::BackDirection = FFaceToDirection(EFaceDirection::Back, FIntVector(-1, 0, 0));
FFaceToDirection FFaceToDirection::FrontDirection = FFaceToDirection(EFaceDirection::Front, FIntVector(1, 0, 0));
FFaceToDirection FFaceToDirection::LeftDirection = FFaceToDirection(EFaceDirection::Left, FIntVector(0, -1, 0));
FFaceToDirection FFaceToDirection::RightDirection = FFaceToDirection(EFaceDirection::Right, FIntVector(0, 1, 0));
