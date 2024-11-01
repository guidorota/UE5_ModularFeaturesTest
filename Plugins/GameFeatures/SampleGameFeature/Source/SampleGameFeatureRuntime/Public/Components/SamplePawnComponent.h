#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "SamplePawnComponent.generated.h"

class UStaticMesh;

/*
 * Sample PawnComponent that simply adds a static mesh to the owning Actor.
 */
UCLASS()
class SAMPLEGAMEFEATURERUNTIME_API USamplePawnComponent : public UPawnComponent
{
	GENERATED_BODY()

public:

	USamplePawnComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMesh> StaticMesh;
};
