#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "ChunkRMCActor.generated.h"

UCLASS()
class RUNDIRECTIONALMESHINGEXTENDED_API AChunkRMCActor : public AActor
{
	GENERATED_BODY()

public:
	AChunkRMCActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	void ClearMesh() const;
	void PrepareMesh() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
