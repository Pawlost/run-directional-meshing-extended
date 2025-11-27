#pragma once
#include "CoreMinimal.h"
#include "ChunkEdit.h"
#include "VoxelGeneratorBase.h"
#include "Chunk/ChunkActor.h"
#include "Chunk/MesherVariables.h"
#include "VoxelMesher/MeshingUtils/RLEVoxelEdit.h"
#include "ChunkSpawnerBase.generated.h"

UCLASS(ClassGroup=(ChunkSpawners), Abstract, Blueprintable)
class RDMCHUNKSPAWNERS_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowAbstract="false", BlueprintBaseOnly), NoClear,
		Category="Chunk")
	TSubclassOf<UBaseVoxelData> VoxelGeneratorBlueprint = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chunk")
	bool UseWorldCenter = false;

	UPROPERTY(VisibleAnywhere)
	FIntVector CenterGridPosition;

	UFUNCTION(BlueprintCallable)
	double GetHighestElevationAtLocation(const FVector& Location) const;

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelAtHit(const FVector& HitPosition, const FVector& HitNormal,
	                      const FName& VoxelName, bool bPick);

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelSphereAtHit(const FVector& HitPosition, const FVector& HitNormal,
	                            const FName& VoxelName, bool bPick, int Radius);

	UFUNCTION(BlueprintCallable)
	void ChangeVoxelCrossNeighborhoodAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName,
	                                       bool bPick);

	UFUNCTION(BlueprintCallable)
	void AddVoxelCrossNeighborhoodToChunkEdit(UPARAM(ref)
	                                          TMap<FIntVector, FChunkEdit>& ChunkEdit,
	                                          const FIntVector& GlobalVoxelPosition,
	                                          const FName& VoxelType);

	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelsInChunk(UPARAM(ref)
		TMap<FIntVector, FChunkEdit>& ChunkEdits);

	virtual TSharedFuture<void> SpawnChunksAsync() PURE_VIRTUAL(AChunkSpawnerBase::SpawnChunks,
	                                                            return TSharedFuture<void>();)

	UFUNCTION(BlueprintCallable)
	FIntVector CalculateGlobalVoxelPositionFromHit(const FVector& HitPosition, const FVector& HitNormal,
	                                               const bool bInnerVoxelPosition) const;

	bool IsInitialized() const;

protected:
	virtual void BeginPlay() override;

	static void AddSideChunk(FMesherVariables& MeshVar, EFaceDirection Direction,
	                         const TSharedPtr<FChunk>& Chunk);

	void AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
	                    const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution = nullptr) const;

	// Wait for all futures
	static void WaitForAllTasks(TArray<TSharedFuture<void>>& Tasks);

	void SpawnAndMoveChunkActor(const TSharedPtr<FChunkParams>& ChunkParams,
	                            TWeakObjectPtr<AChunkActor>& OutActorPtr) const;

	void AddGlobalVoxelPositionToEdit(TMap<FIntVector, FChunkEdit>& OutChunkEdit, const FIntVector& GlobalVoxelPosition,
	                                  const FName& VoxelType) const;

	FIntVector GetChunkGridPositionFromGlobalPosition(const FVector& GlobalPosition) const;

	virtual void ApplyVoxelChanges(TMap<FIntVector, TArray<FRLEVoxelEdit>>& EditChunk) PURE_VIRTUAL(
		AChunkSpawnerBase::ChangeVoxelInChunk)
	
	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;
	bool bIsInitialized = false;

private:
	bool CheckVoxelBoundary(const FIntVector& VoxelPosition) const;
};
