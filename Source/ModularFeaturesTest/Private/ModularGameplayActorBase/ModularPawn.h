#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ModularPawn.generated.h"

/*
 * Base class that automatically registers this Actor as a UGameFrameworkComponentManager receiver.
 */
UCLASS()
class AModularPawn : public APawn
{
	GENERATED_BODY()

public:

	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
};