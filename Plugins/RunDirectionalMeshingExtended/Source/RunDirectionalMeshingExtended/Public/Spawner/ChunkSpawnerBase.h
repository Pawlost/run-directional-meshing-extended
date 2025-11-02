#pragma once
#include "CoreMinimal.h"
#include "VoxelMesher/MeshingUtils/MesherVariables.h"
#include "VoxelMesher/MeshingUtils/VoxelChange.h"
#include "ChunkSpawnerBase.generated.h"

struct FVoxelPosition;
enum EFaceDirection : uint8;
struct FChunk;
struct FMesherVariables;
class UVoxelGeneratorBase;

UCLASS(ClassGroup=(ChunkSpawners), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGEXTENDED_API AChunkSpawnerBase : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowAbstract="false", BlueprintBaseOnly), NoClear,
		Category="Chunk")
	TSubclassOf<UVoxelGeneratorBase> VoxelGeneratorBlueprint = nullptr;

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

	void ChangeVoxelCrossNeighborhoodAtHit(const FVector& HitPosition, const FVector& HitNormal, const FName& VoxelName, bool bPick);
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelsInChunk(FCrossChunkEdit& ChunkEdits) PURE_VIRTUAL(AChunkSpawnerBase::ChangeVoxelInChunk)
	
	virtual TSharedFuture<void> SpawnChunksAsync() PURE_VIRTUAL(AChunkSpawnerBase::SpawnChunks, return TSharedFuture<void>();)
	
	UFUNCTION(BlueprintCallable)
	FIntVector CalculateGlobalVoxelPositionFromHit(const FVector& HitPosition,const FVector& HitNormal, const bool bInnerVoxelPosition) const;

	bool IsInitialized() const;
protected:
	
	virtual void BeginPlay() override;

	static void AddSideChunk(FMesherVariables& MeshVar, EFaceDirection Direction,
	                         const TSharedPtr<FChunk>& Chunk);

	void AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
	                    const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution = nullptr) const;

	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;
	
	// Wait for all futures
	static void WaitForAllTasks(TArray<TSharedFuture<void>>& Tasks);
	
	void SpawnAndMoveChunkActor(const TSharedPtr<FChunkParams>& ChunkParams, TWeakObjectPtr<AChunkActor>& OutActorPtr) const;

	void AddGlobalVoxelPositionToEdit(FCrossChunkEdit& OutChunkEdit, const FIntVector& GlobalVoxelPosition, const FName& VoxelType) const;

	FIntVector GetChunkGridPositionFromGlobalPosition(const FVector& GlobalPosition) const;
	
	bool bIsInitialized = false;

private:
	bool CheckVoxelBoundary(const FIntVector& VoxelPosition) const;
};
