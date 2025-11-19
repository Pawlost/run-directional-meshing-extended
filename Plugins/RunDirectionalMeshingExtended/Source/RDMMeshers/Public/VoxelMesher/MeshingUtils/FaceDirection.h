#pragma once
#include "FaceDirection.generated.h"

UENUM()
enum EFaceDirection : uint8
{
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Bottom = 4,
	Top = 5
};