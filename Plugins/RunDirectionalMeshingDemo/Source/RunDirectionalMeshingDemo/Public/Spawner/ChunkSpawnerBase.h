#pragma once
#include "CoreMinimal.h"
#include "Voxel/VoxelPosition.h"
#include "ChunkSpawnerBase.generated.h"

struct FVoxelPosition;
enum class EFaceDirection : uint8;
struct FChunk;
struct FMesherVariables;
class UVoxelGeneratorBase;

UCLASS(ClassGroup=(ChunkSpawners), Abstract, Blueprintable)
class RUNDIRECTIONALMESHINGDEMO_API AChunkSpawnerBase : public AActor
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
	FName GetVoxelNameAtHit(const FVector& HitPosition, const FVector& HitNormal);
	
	UFUNCTION(BlueprintCallable)
	virtual void ChangeVoxelInChunk(const FVoxelPosition& VoxelPosition,
	                                const FName& VoxelName) PURE_VIRTUAL(AChunkSpawnerBase::ChangeVoxelInChunk)
	
	UFUNCTION(BlueprintCallable)
	virtual FName GetVoxelFromChunk(const FVoxelPosition& VoxelPosition) PURE_VIRTUAL(AChunkSpawnerBase::GetVoxelFromChunk, return "";)
	
	virtual TSharedFuture<void> SpawnChunksAsync() PURE_VIRTUAL(AChunkSpawnerBase::SpawnChunks, return TSharedFuture<void>();) 

	bool IsInitialized() const;
protected:
	virtual void BeginPlay() override;

	static void AddSideChunk(FMesherVariables& MeshVar, EFaceDirection Direction,
	                         const TSharedPtr<FChunk>& Chunk);

	void AddChunkToGrid(TSharedPtr<FChunk>& Chunk,
	                    const FIntVector& GridPosition, TSharedFuture<void>* AsyncExecution = nullptr) const;

	UPROPERTY()
	TObjectPtr<UVoxelGeneratorBase> VoxelGenerator;

	FIntVector WorldPositionToChunkGridPosition(const FVector& WorldPosition) const;

	FVoxelPosition CalculateVoxelPosition(const FVector& HitPosition, const FVector& AdjustedNormal) const;
	
	// Wait for all futures
	static void WaitForAllTasks(TArray<TSharedFuture<void>>& Tasks);

	bool bIsInitialized = false;
};
