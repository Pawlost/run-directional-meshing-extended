#pragma once

#include "CoreMinimal.h"
#include "RealtimeMeshCollision.h"
#include "RealtimeMeshConfig.h"
#include "ChunkRMCActor.generated.h"

UCLASS()
class RUNDIRECTIONALMESHINGDEMO_API AChunkRMCActor : public AActor
{
	GENERATED_BODY()

public:
	AChunkRMCActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<URealtimeMeshComponent> RealtimeMeshComponent;

	void ClearMesh() const;
	void PrepareMesh() const;

	const FRealtimeMeshSectionGroupKey GroupKey;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FRealtimeMeshCollisionConfiguration DefaultConfig;
};
