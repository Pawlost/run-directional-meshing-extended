#pragma once
#include "BorderVisualizationOption.generated.h"

UENUM(BlueprintType)
enum class EBorderVisualizationOption : uint8
{
	All,
	None,
	OnlyOuterBorders,
};
