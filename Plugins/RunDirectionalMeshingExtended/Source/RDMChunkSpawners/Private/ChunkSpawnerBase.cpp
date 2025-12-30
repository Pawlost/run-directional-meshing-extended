#include "ChunkSpawnerBase.h"

#include "BaseVoxelData.h"
#include "VoxelGeneratorBase.h"

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

void AChunkSpawnerBase::SpawnChunkActors(const TSharedRef<FMesherVariables>& Spawner) const
{
	//Creating AsyncTask from main thread will cause deadlock
	const auto Chunk = Spawner->OriginalChunk;
	SpawnAndMoveChunkActor(Spawner, Chunk->ChunkMeshActor);
	
	for (int i = 0; i < CHUNK_FACE_COUNT; ++i)
	{
		SpawnAndMoveChunkActor(Spawner, Chunk->BorderChunkMeshActor[i]);
	}
}

double AChunkSpawnerBase::GetHighestElevationAtLocation(const FVector& Location) const
{
	return VoxelGenerator->GetHighestElevationAtLocation(Location);
}

void AChunkSpawnerBase::ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName,
                                         const bool bPick)
{
	const auto GlobalVoxelPosition = CalculateGlobalVoxelPositionFromHit(HitPosition, HitNormal, bPick);
	TMap<FIntVector, FChunkEdit> ChunkEdit;
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition, VoxelName);
	ChangeVoxelsInChunk(ChunkEdit);
}

void AChunkSpawnerBase::ChangeVoxelSphereAtHit(const FVector& HitPosition, const FVector& HitNormal,
                                               const FName& VoxelName, bool bPick, int Radius)
{
	TMap<FIntVector, TArray<FVoxelChange>> Sphere;
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

	TMap<FIntVector, FChunkEdit> ChunkEdit;

	AddVoxelCrossNeighborhoodToChunkEdit(ChunkEdit, GlobalVoxelPosition, VoxelName);

	ChangeVoxelsInChunk(ChunkEdit);
}

void AChunkSpawnerBase::AddVoxelCrossNeighborhoodToChunkEdit(TMap<FIntVector, FChunkEdit>& ChunkEdit,
                                                             const FIntVector& GlobalVoxelPosition,
                                                             const FName& VoxelType)
{
	// Middle
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, 0), VoxelType);

	// Left
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, -1, 0), VoxelType);

	// Right
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 1, 0), VoxelType);

	// Bottom
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, -1), VoxelType);

	// Top
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(0, 0, 1), VoxelType);

	// Back
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(-1, 0, 0), VoxelType);

	// Front
	AddGlobalVoxelPositionToEdit(ChunkEdit, GlobalVoxelPosition + FIntVector(1, 0, 0), VoxelType);
}

// Only one is supported
void AChunkSpawnerBase::ChangeVoxelsInChunk(TMap<FIntVector, FChunkEdit>& ChunkEdits)
{
	TMap<FIntVector, TArray<FRLEVoxelEdit>> ChunkVoxelEdits;
	
	//TODO: try to make it faster
	
	// Voxel modifications must always be sorted using this coordinate logic
	for (auto& ChunkEdit : ChunkEdits)
	{
		auto VoxelChanges = ChunkEdit.Value.VoxelChanges;
		
		VoxelChanges.Sort([](const FVoxelChange& A, const FVoxelChange& B)
		{
			if (A.VoxelPosition.X != B.VoxelPosition.X)
				return A.VoxelPosition.X < B.VoxelPosition.X;
			if (A.VoxelPosition.Z != B.VoxelPosition.Z)
				return A.VoxelPosition.Z < B.VoxelPosition.Z;
			return A.VoxelPosition.Y < B.VoxelPosition.Y;
		});
		
		TArray<FRLEVoxelEdit> VoxelEdits;
		
		while (!VoxelChanges.IsEmpty())
		{
			auto VoxelChange = VoxelChanges.Pop();
			auto EditVoxel =  FRLEVoxel{1, VoxelGenerator->GetVoxelByName(VoxelChange.VoxelName)};

			while (!VoxelChanges.IsEmpty() && (VoxelChanges.Top().VoxelPosition +
					FIntVector(0, 1, 0)) == VoxelChange.VoxelPosition)
			{
				VoxelChange = VoxelChanges.Pop();
				EditVoxel.RunLenght++;
			}
			
			FRLEVoxelEdit VoxelEdit { VoxelGenerator->CalculateVoxelIndex(VoxelChange.VoxelPosition), EditVoxel};
			
			VoxelEdits.Add(VoxelEdit);
		}
			
		ChunkVoxelEdits.Add(ChunkEdit.Key, VoxelEdits);
	}

	ApplyVoxelChanges(ChunkVoxelEdits);
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
	return FIntVector(FMath::Floor(VoxelPosition.X), FMath::Floor(VoxelPosition.Y), FMath::Floor(VoxelPosition.Z)) -
		AdjustedNormal;
}

void AChunkSpawnerBase::AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
                                       const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution) const
{
	Chunk->GridPosition = GridPosition;
	checkf(VoxelMesherBlueprint, TEXT("VoxelMesherBlueprint is required but was not set"));
	Chunk->InitVoxelMesher(VoxelMesherBlueprint, VoxelGenerator);

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


void AChunkSpawnerBase::SpawnAndMoveChunkActor(const TSharedPtr<FMesherVariables>& ChunkParams,
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

void AChunkSpawnerBase::AddGlobalVoxelPositionToEdit(TMap<FIntVector, FChunkEdit>& OutChunkEdit,
                                                     const FIntVector& GlobalVoxelPosition,
                                                     const FName& VoxelType) const
{
	const auto ChunkPosition = GetChunkGridPositionFromGlobalPosition(FVector(GlobalVoxelPosition));
	const FIntVector VoxelPosition = FIntVector(
		GlobalVoxelPosition - (ChunkPosition * VoxelGenerator->GetVoxelCountPerVoxelLine()));

	const FVoxelChange Modification(VoxelType, VoxelPosition);
	auto& VoxelModifications = OutChunkEdit.FindOrAdd(ChunkPosition);
	VoxelModifications.VoxelChanges.Add(Modification);
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
