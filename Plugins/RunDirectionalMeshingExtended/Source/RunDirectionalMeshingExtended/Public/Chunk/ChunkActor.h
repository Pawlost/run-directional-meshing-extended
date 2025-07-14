#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "ChunkActor.generated.h"

UCLASS()
class RUNDIRECTIONALMESHINGEXTENDED_API AChunkActor : public AActor
{
	GENERATED_BODY()

public:
	AChunkActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="RealtimeMesh")
	TObjectPtr<UProceduralMeshComponent> ProceduralMeshComponent;

	void ClearMesh() const;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
