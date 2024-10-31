#include "ModularGameplayActorBase/ModularPawn.h"

#include "Components/GameFrameworkComponentManager.h"

void AModularPawn::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularPawn::BeginPlay()
{

	/*
	 * As indicated in the UE documentation, "All extensible games are expected to send this event at the appropriate actor-specific point, as plugins may be listening for it.".
	 *
	 * `UGameFeatureAction`s rely on this event to perform their operation.
	 */
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	
	Super::BeginPlay();
}

void AModularPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	
	Super::EndPlay(EndPlayReason);
}
