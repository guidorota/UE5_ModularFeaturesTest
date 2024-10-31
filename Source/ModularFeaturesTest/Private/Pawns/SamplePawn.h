#pragma once

#include "CoreMinimal.h"
#include "ModularGameplayActorBase/ModularPawn.h"
#include "SamplePawn.generated.h"

class UCapsuleComponent;

UCLASS()
class ASamplePawn : public AModularPawn
{
	GENERATED_BODY()

public:
	ASamplePawn();

private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UCapsuleComponent> CapsuleComponent;
};
