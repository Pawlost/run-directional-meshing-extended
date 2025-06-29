#pragma once

#define CHUNK_FACE_COUNT 6

UENUM()
enum class EFaceDirection : uint8
{
	Front = 0,
	Back = 1,
	Right = 2,
	Left = 3,
	Bottom = 4,
	Top = 5
};

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