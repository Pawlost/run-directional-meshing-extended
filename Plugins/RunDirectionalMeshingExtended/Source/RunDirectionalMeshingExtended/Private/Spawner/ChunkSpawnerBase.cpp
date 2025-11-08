#include "Spawner/ChunkSpawnerBase.h"

#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "Voxel/Generator/VoxelGeneratorBase.h"

bool AChunkSpawnerBase::IsInitialized() const
{
	return bIsInitialized;
}

void AChunkSpawnerBase::BeginPlay()
{
	// Check if the template is valid
	if (VoxelGeneratorBlueprint)
	{
		// Create the component
		VoxelGenerator = NewObject<UVoxelGeneratorBase>(this, VoxelGeneratorBlueprint);

		if (VoxelGenerator)
		{
			// Register the component (required for it to work properly)
			VoxelGenerator->RegisterComponent();
		}
	}

	checkf(VoxelGenerator, TEXT("Voxel generator must be valid"));

	bIsInitialized = true;
	
	Super::BeginPlay();
}

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& Location) const
{
	return VoxelGenerator->GetHighestElevationAtLocation(Location);
}

void AChunkSpawnerBase::ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName,
                                         const bool bPick)
{
	const auto GlobalVoxelPosition = CalculateGlobalVoxelPositionFromHit(HitPosition, HitNormal, bPick);
	FCrossChunkEdit ChunkEdit;
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition, VoxelName);
	ChangeVoxelsInChunk(ChunkEdit);
}

void AChunkSpawnerBase::ChangeVoxelSphereAtHit(const FVector& HitPosition, const FVector& HitNormal,
                                               const FName& VoxelName, bool bPick, int Radius)
{
	TMap<FIntVector, TArray<FVoxelEdit>> Sphere;
	const auto VoxelPosition = CalculateGlobalVoxelPositionFromHit(HitPosition, HitNormal, bPick);

	// TODO: finish
	for (int r = 0; r < Radius; r++)
	{
	}
}


void AChunkSpawnerBase::ChangeVoxelCrossNeighborhoodAtHit(const FVector& HitPosition, const FVector& HitNormal,
                                                          const FName& VoxelName, bool bPick)
{
	auto GlobalVoxelPosition = CalculateGlobalVoxelPositionFromHit(HitPosition, HitNormal, bPick);

	FCrossChunkEdit ChunkEdit;

	// Middle
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, 0), VoxelName);
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 1, 0), VoxelName);
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 2, 0), VoxelName);

	// Left
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, -1, 0), VoxelName);

	// Right

	// Bottom
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, -1), VoxelName);

	// Top
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, 1), VoxelName);

	// Back
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(-1, 0, 0), VoxelName);

	// Front
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(1, 0, 0), VoxelName);

	ChangeVoxelsInChunk(ChunkEdit);
}

FIntVector AChunkSpawnerBase::CalculateGlobalVoxelPositionFromHit(const FVector& HitPosition, const FVector& HitNormal,
                                                                  const bool bInnerVoxelPosition) const
{
	FIntVector AdjustedNormal;

	if (bInnerVoxelPosition)
	{
		// Inner Voxel position
		AdjustedNormal.X = FMath::Clamp(HitNormal.X, 0, 1);
		AdjustedNormal.Y = FMath::Clamp(HitNormal.Y, 0, 1);
		AdjustedNormal.Z = FMath::Clamp(HitNormal.Z, 0, 1);
	}
	else
	{
		// Outer Voxel position
		AdjustedNormal.X = -FMath::Clamp(HitNormal.X, -1, 0);
		AdjustedNormal.Y = -FMath::Clamp(HitNormal.Y, -1, 0);
		AdjustedNormal.Z = -FMath::Clamp(HitNormal.Z, -1, 0);
	}

	FMatrix ActorLocationMatrix = FTranslationMatrix(FVector(0));

	if (!UseWorldCenter)
	{
		// Transform hit position to local coordinates, if they are enabled
		ActorLocationMatrix = FTranslationMatrix(GetActorLocation()).Inverse();
	}

	ActorLocationMatrix = ActorLocationMatrix * FScaleMatrix(FVector(VoxelGenerator->GetVoxelSize())).Inverse();
	const FVector VoxelPosition = ActorLocationMatrix.TransformPosition(HitPosition);

	// Flooring instead of rounding improves precision, subtracted normal from global voxel coordinates
	return FIntVector(FMath::Floor(VoxelPosition.X), FMath::Floor(VoxelPosition.Y), FMath::Floor(VoxelPosition.Z)) - AdjustedNormal;
}

void AChunkSpawnerBase::AddSideChunk(FMesherVariables& MeshVar, EFaceDirection Direction,
                                     const TSharedPtr<FChunk>& Chunk)
{
	const auto DirectionIndex = static_cast<uint8>(Direction);
	MeshVar.ChunkParams.SideChunks[DirectionIndex] = Chunk.IsValid() ? Chunk : nullptr;
}

void AChunkSpawnerBase::AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
                                       const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution) const
{
	Chunk->GridPosition = GridPosition;

	if (AsyncExecution != nullptr)
	{
		// Generate voxels on async thread if promise is expected
		*AsyncExecution = Async(EAsyncExecution::ThreadPool, [this, Chunk]()
		{
#if CPUPROFILERTRACE_ENABLED
			TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation");
#endif

			VoxelGenerator->GenerateVoxels(*Chunk.Get());
		}).Share();
	}
	else
	{
#if CPUPROFILERTRACE_ENABLED
		TRACE_CPUPROFILER_EVENT_SCOPE("Voxel generation");
#endif

		VoxelGenerator->GenerateVoxels(*Chunk);
	}
}

void AChunkSpawnerBase::WaitForAllTasks(TArray<TSharedFuture<void>>& Tasks)
{
	for (auto Task : Tasks)
	{
		if (Task.IsValid() && !Task.IsReady())
		{
			Task.Wait();
		}
	}

	Tasks.Empty();
}


void AChunkSpawnerBase::SpawnAndMoveChunkActor(const TSharedPtr<FChunkParams>& ChunkParams,
                                               TWeakObjectPtr<AChunkActor>& OutActorPtr) const
{
	const auto World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	//Spawn actor
	const auto Chunk = ChunkParams->OriginalChunk;
	const auto SpawnLocation = FVector(Chunk->GridPosition) * VoxelGenerator->GetChunkSpacing();

	FAttachmentTransformRules ActorAttachmentRules = FAttachmentTransformRules::KeepWorldTransform;
	if (!ChunkParams->WorldTransform)
	{
		ActorAttachmentRules = FAttachmentTransformRules::KeepRelativeTransform;
	}

	if (OutActorPtr == nullptr)
	{
		// If there is no actor spawn new one.
		OutActorPtr = World->SpawnActor<AChunkActor>(AChunkActor::StaticClass(), SpawnLocation,
		                                             FRotator::ZeroRotator);

		if (!OutActorPtr.IsValid() || !ChunkParams->SpawnerPtr.IsValid())
		{
			return;
		}

		OutActorPtr->AttachToActor(ChunkParams->SpawnerPtr.Get(), ActorAttachmentRules);
	}
	else
	{
		if (!OutActorPtr.IsValid())
		{
			return;
		}

		// If actor exists, ensure correct location
		if (!ChunkParams->WorldTransform)
		{
			OutActorPtr->SetActorRelativeLocation(SpawnLocation);
		}
		else
		{
			OutActorPtr->SetActorLocation(SpawnLocation);
		}
		OutActorPtr->ClearMesh();
	}
}

void AChunkSpawnerBase::AddGlobalVoxelPositionToEdit(FCrossChunkEdit& OutChunkEdit,
                                                     const FIntVector& GlobalVoxelPosition,
                                                     const FName& VoxelType) const
{
	const auto ChunkPosition = GetChunkGridPositionFromGlobalPosition(FVector(GlobalVoxelPosition));
	const FIntVector VoxelPosition = FIntVector(
		GlobalVoxelPosition - (ChunkPosition * VoxelGenerator->GetVoxelCountPerVoxelLine()));
	OutChunkEdit.AddVoxelEdit(VoxelPosition, ChunkPosition, VoxelType);
}

FIntVector AChunkSpawnerBase::GetChunkGridPositionFromGlobalPosition(const FVector& GlobalPosition) const
{
	const auto ImpreciseChunkPosition = GlobalPosition / VoxelGenerator->GetVoxelCountPerVoxelLine();
	// Floor is used to adjust negative numbers
	return FIntVector(FMath::Floor(ImpreciseChunkPosition.X), FMath::Floor(ImpreciseChunkPosition.Y),
	                  FMath::Floor(ImpreciseChunkPosition.Z));
}

bool AChunkSpawnerBase::CheckVoxelBoundary(const FIntVector& VoxelPosition) const
{
	const int ChunkDimensions = VoxelGenerator->GetVoxelCountPerVoxelLine();
	return VoxelPosition.X < 0 || VoxelPosition.Y < 0 || VoxelPosition.Z < 0 ||
		VoxelPosition.X >= ChunkDimensions || VoxelPosition.Y >= ChunkDimensions || VoxelPosition.Z >= ChunkDimensions;
}
