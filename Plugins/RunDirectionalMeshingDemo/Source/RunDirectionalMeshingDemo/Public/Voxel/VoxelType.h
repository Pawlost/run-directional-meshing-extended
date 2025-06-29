#pragma once

#include "CoreMinimal.h"
#include "FastNoiseWrapper.h"
#include "VoxelType.generated.h"

USTRUCT(BlueprintType)
struct RUNDIRECTIONALMESHINGDEMO_API FVoxelType : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (ToolTip = "Material applied to the generated voxel."))
	TObjectPtr<UMaterial> Material = nullptr;

	UPROPERTY(EditAnywhere, Category = "Basic",
	meta = (ToolTip = "If false, this voxel will not be generated as part of the terrain but will remain placeable and interactable."))
	bool bGenerateNoise = true;

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (ToolTip =
			"Specifies if the voxel surface is transparent. When true, additional mesh borders are created for opaque voxels."
		))
	bool bIsTransparent = false;

	UPROPERTY(EditAnywhere, Category = "Basic",
		meta = (EditCondition = "bGenerateNoise", ToolTip =
			"Determines whether generation of this voxel type stops at a certain depth. Useful for creating floating islands, caves or surface layers."
		))
	bool bGenerateReversedSurface = false;

	UPROPERTY(EditAnywhere, Category = "Surface",
		meta = (EditCondition = "bGenerateNoise", ToolTip =
			"Defines the minimum and maximum elevation of the surface."
		))
	double Surface_Elevation = 100.0;

	UPROPERTY(EditAnywhere, Category = "Surface",
		meta = (EditCondition = "bGenerateNoise", ToolTip =
			"Defines the additional distance from sea level at which the surface is positioned. Sea level is assumed to be at 0."
		))
	int64 Surface_DistanceFromSeaLevel = 20;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bGenerateNoise"))
	int32 SurfaceNoise_Seed = 1234;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bGenerateNoise"))
	EFastNoise_NoiseType SurfaceNoise_Type = EFastNoise_NoiseType::ValueFractal;

	UPROPERTY(EditAnywhere, Category = "SurfaceNoise", meta = (EditCondition = "bGenerateNoise"))
	float SurfaceNoise_Frequency = 0.007f;

	UPROPERTY(EditAnywhere, Category = "ReversedSurface",
		meta = (EditCondition = "bGenerateReversedSurface && bGenerateNoise", ToolTip =
			"Defines the minimum and maximum depth of the reversed surface."
		))
	double ReversedSurface_Depth = 150.0;

	UPROPERTY(EditAnywhere, Category = "ReversedSurface",
		meta = (EditCondition = "bGenerateReversedSurface && bGenerateNoise", ToolTip =
			"Defines the additional distance from sea level at which the surface is positioned. Sea level is assumed to be at 0. Generation of reversed surface must be enabled."
		))
	int64 ReversedSurface_DistanceFromSeaLevel = -20;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bGenerateNoise"))
	int32 ReversedSurfaceNoise_Seed = 1234;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bGenerateNoise"))
	EFastNoise_NoiseType ReversedSurfaceNoise_Type = EFastNoise_NoiseType::ValueFractal;

	UPROPERTY(EditAnywhere, Category = "ReversedSurfaceNoise",
		meta = (EditCondition = "bGenerateReversedSurface && bGenerateNoise"))
	float ReversedSurfaceNoise_Frequency = 0.05f;
};
