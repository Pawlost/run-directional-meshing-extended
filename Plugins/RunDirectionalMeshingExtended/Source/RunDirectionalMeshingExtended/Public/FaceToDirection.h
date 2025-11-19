#pragma once
#include "VoxelMesher/MeshingUtils/FaceDirection.h"

struct FFaceToDirection
{
	const EFaceDirection FaceSide;
	const FIntVector Direction;
	
	static FFaceToDirection FrontDirection;
	static FFaceToDirection BackDirection;
	static FFaceToDirection LeftDirection;
	static FFaceToDirection RightDirection;
	static FFaceToDirection TopDirection;
	static FFaceToDirection BottomDirection;
};
