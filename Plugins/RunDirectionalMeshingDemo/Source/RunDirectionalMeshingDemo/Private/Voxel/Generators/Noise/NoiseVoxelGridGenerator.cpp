#include "Voxel/Generator/Noise/NoiseVoxelGridGenerator.h"

#include "Chunk/Chunk.h"
#include "Voxel/Generator/Noise/NoiseSurfaceGenerator.h"
#include "VoxelMesher/VoxelMesherBase.h"

void UNoiseVoxelGridGenerator::BeginPlay()
{
	Super::BeginPlay();

	checkf(VoxelTypeTable, TEXT("Voxel table must be set"));

	for (const FName VoxelName : VoxelTypeTable->GetRowNames())
	{
		// Voxel type is copied from table for consistency
		const auto VoxelType = *VoxelTypeTable->FindRow<FVoxelType>(VoxelName, "");

		// Initialize all values for surface noise
		auto SurfaceNoise = NewObject<UFastNoiseWrapper>(this);
		auto ReversedSurfaceNoise = NewObject<UFastNoiseWrapper>(this);

		SurfaceNoise->SetupFastNoise(VoxelType.SurfaceNoise_Type, VoxelType.SurfaceNoise_Seed,
		                             VoxelType.SurfaceNoise_Frequency);

		ReversedSurfaceNoise->SetupFastNoise(VoxelType.ReversedSurfaceNoise_Type,
		                                     VoxelType.ReversedSurfaceNoise_Seed,
		                                     VoxelType.ReversedSurfaceNoise_Frequency);
		
		auto SurfaceGenerator = FNoiseSurfaceGenerator(SurfaceNoise,
		                                               ReversedSurfaceNoise, VoxelName, VoxelType);

		SurfaceGenerators.Add(SurfaceGenerator);
	}
}

void UNoiseVoxelGridGenerator::GenerateVoxels(FChunk& Chunk)
{
	//NOTICE: This generation is unoptimized because it is not major part of my bachelor's thesis
	const auto ChunkDimension = GetVoxelCountPerChunkDimension();
	const auto VoxelTypeCount = SurfaceGenerators.Num();

	const auto GridPos = Chunk.GridPosition * ChunkDimension;

	if (IsChunkPositionOutOfBounds(GridPos.Z, GridPos.Z + ChunkDimension - 1))
	{
		// Skip all chunks outside possible noise fill
		return;
	}

	TArray<FVoxel> VoxelGrid;
	VoxelGrid.SetNum(GetVoxelCountPerChunk());

	// Iteration through voxel grid (voxel model)
	for (uint16 x = 0; x < ChunkDimension; x++)
	{
		for (uint16 y = 0; y < ChunkDimension; y++)
		{
			for (uint16 z = 0; z < ChunkDimension; z++)
			{
				const auto Index = CalculateVoxelIndex(x, y, z);
				// In case no voxel is added make it empty
				// This is important in case of reusing chunks
				//	VoxelGrid[Index] = FVoxel();

				// VoxelId is given by position of surface generator in array.
				// Generators keep rewriting position and last Voxel will rewrite the rest
				for (int32 VoxelId = 0; VoxelId < VoxelTypeCount; VoxelId++)
				{
					auto SurfaceGenerator = SurfaceGenerators[VoxelId];

					if (!IsValid(SurfaceGenerator.SurfaceGenerator))
					{
						return;
					}

					if (!SurfaceGenerator.VoxelType.bGenerateNoise)
					{
						// Skip all voxel types that should not be generated
						continue;
					}

					// Initialize voxel with given id and set transparency
					auto Voxel = FVoxel(VoxelId, SurfaceGenerator.VoxelType.bIsTransparent);

					// Calculate scene position using chunk and voxel grid coordinates
					const float PosX = x + GridPos.X;
					const float PosY = y + GridPos.Y;
					const double CurrentElevation = GridPos.Z + z;

					// Always get surface elevation
					const double Elevation = ComputeSurfaceGradient(PosX, PosY, SurfaceGenerator.SurfaceGenerator,
																	SurfaceGenerator.VoxelType.Surface_Elevation,
																	SurfaceGenerator.VoxelType.
																	Surface_DistanceFromSeaLevel);
					bool AddVoxel;
					
					if (SurfaceGenerator.VoxelType.bGenerateReversedSurface)
					{
						if (!IsValid(SurfaceGenerator.ReverseSurfaceGenerator))
						{
							return;
						}

						const double Depth = ComputeSurfaceGradient(
							PosX, PosY, SurfaceGenerator.ReverseSurfaceGenerator,
							SurfaceGenerator.VoxelType.ReversedSurface_Depth,
							SurfaceGenerator.VoxelType.ReversedSurface_DistanceFromSeaLevel);

						// Create a span between depth and elevation where voxel may be generated, used to create floating islands
						AddVoxel = CurrentElevation > -Depth && CurrentElevation < Elevation;
					}
					else
					{
						// Depth may be infinite if the lowest point is not defined
						AddVoxel = CurrentElevation < Elevation;
					}

					if (AddVoxel)
					{
						// Rewrite voxel at index from previous value
						ChangeKnownVoxelAtIndex(VoxelGrid, Chunk.ChunkVoxelIdTable, Index, Voxel);
					}
				}
			}
		}
	}

	VoxelMesher->CompressVoxelGrid(Chunk,VoxelGrid);
}

double UNoiseVoxelGridGenerator::GetHighestElevationAtLocation(const FVector& Location)
{
	double MaxElevation = 0.0;

	for (int32 VoxelId = 0; VoxelId < SurfaceGenerators.Num(); VoxelId++)
	{
		auto SurfaceGenerator = SurfaceGenerators[VoxelId];
		const auto Elevation = ComputeSurfaceGradient(Location.X, Location.Y, SurfaceGenerator.SurfaceGenerator,
		                                              SurfaceGenerator.VoxelType.Surface_Elevation,
		                                              SurfaceGenerator.VoxelType.Surface_DistanceFromSeaLevel);

		// Generate gradients at position, since noise is pseudorandom number generator using seed the values will be the same

		MaxElevation = FMath::Max(Elevation, MaxElevation);
	}

	return MaxElevation * VoxelSize;
}

TTuple<FName, FVoxelType> UNoiseVoxelGridGenerator::GetVoxelTypeById(const int32& VoxelId) const
{
	auto SurfaceGenerator = SurfaceGenerators[VoxelId];
	return TTuple<FName, FVoxelType>(SurfaceGenerator.VoxelName, SurfaceGenerator.VoxelType);
}

FVoxel UNoiseVoxelGridGenerator::GetVoxelByName(const FName& VoxelName) const
{
	for (int32 Index = 0; Index < SurfaceGenerators.Num(); Index++)
	{
		auto SurfaceGenerator = SurfaceGenerators[Index];
		if (SurfaceGenerator.VoxelName == VoxelName)
		{
			return FVoxel(Index, SurfaceGenerator.VoxelType.bIsTransparent);
		}
	}
	return FVoxel();
}

double UNoiseVoxelGridGenerator::ComputeSurfaceGradient(const float PosX, const float PosY,
                                                        const TObjectPtr<UFastNoiseWrapper>& Generator,
                                                        const double Elevation, const double DistanceFromSurfaceLevel)
{
	return Generator->GetNoise2D(PosX, PosY) * Elevation + DistanceFromSurfaceLevel;
}

bool UNoiseVoxelGridGenerator::IsChunkPositionOutOfBounds(const double MinZPosition, const double MaxZPosition) const
{
	for (const auto Generator : SurfaceGenerators)
	{
		if (MinZPosition < Generator.VoxelType.Surface_Elevation + Generator.VoxelType.Surface_DistanceFromSeaLevel
			|| Generator.VoxelType.bGenerateReversedSurface &&
			MaxZPosition > -Generator.VoxelType.Surface_DistanceFromSeaLevel + Generator.VoxelType.
			ReversedSurface_DistanceFromSeaLevel)
		{
			// If there is a possibility to generate noise gradient in chunk, return false
			return false;
		}
	}

	// Max possible position is out of bounds for all generators
	return true;
}
