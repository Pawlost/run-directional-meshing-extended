#pragma once
#include "FaceDirection.generated.h"

#define VOXEL_FACE_COUNT 6

UENUM()
enum EFaceDirection : uint8
{
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Top = 4,
	Bottom = 5
};