#include "GameFeatures/GameFeatureAction_AddNiagara.h"

#include "Components/GameFrameworkComponentManager.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddNiagara)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddNiagara::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(
		this,
		&UGameFeatureAction_AddNiagara::HandleGameInstanceStart,
		FGameFeatureStateChangeContext(Context));

	for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
	{
		if (Context.ShouldApplyToWorldContext(WorldContext))
		{
			AddToWorld(WorldContext, Context);
		}
	}
}

void UGameFeatureAction_AddNiagara::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	if (const FDelegateHandle *MaybeHandle = GameInstanceStartHandles.Find(Context); ensure(MaybeHandle))
	{
		FWorldDelegates::OnStartGameInstance.Remove(*MaybeHandle);
	}
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddNiagara::IsDataValid(class FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FGameFeatureNiagaraSystemEntry& Entry : NiagaraSystemList)
	{
		if (Entry.ActorClass.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("EntryHasNullActor", "Null ActorClass at index {0} in NiagaraSystemList"), FText::AsNumber(EntryIndex)));
		}

		if (!Entry.NiagaraSystem)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("EntryHasNullNiagaraSystem", "Null NiagaraSystem at index {0} in NiagaraSystemList"), FText::AsNumber(EntryIndex)));
		}

		EntryIndex++;
	}
	
	return Result;
}
#endif

void UGameFeatureAction_AddNiagara::HandleGameInstanceStart(UGameInstance* GameInstance,
                                                            FGameFeatureStateChangeContext ChangeContext)
{
	if (const FWorldContext* WorldContext = GameInstance->GetWorldContext())
	{
		if (ChangeContext.ShouldApplyToWorldContext(*WorldContext))
		{
			AddToWorld(*WorldContext, ChangeContext);
		}
	}
}

void UGameFeatureAction_AddNiagara::AddToWorld(const FWorldContext& WorldContext,
	const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if (GameInstance == nullptr || World == nullptr || !World->IsGameWorld())
	{
		return;
	}

	UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance);
	ensure(ComponentManager);

	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	int32 EntryIndex = 0;
	for (const FGameFeatureNiagaraSystemEntry& Entry : NiagaraSystemList)
	{
		if (Entry.ActorClass.IsNull())
		{
			continue;
		}

		UGameFrameworkComponentManager::FExtensionHandlerDelegate AddNiagaraSystemDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
			this,
			&UGameFeatureAction_AddNiagara::HandleActorExtension,
			EntryIndex,
			ChangeContext);

		TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentManager->AddExtensionHandler(Entry.ActorClass, AddNiagaraSystemDelegate);
		ActiveData.ComponentRequests.Add(ExtensionRequestHandle);

		EntryIndex++;
	}
}

void UGameFeatureAction_AddNiagara::HandleActorExtension(AActor* Actor, FName EventName, int32 entryIndex,
	FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData* ActiveData = ContextData.Find(ChangeContext);
	if (ActiveData == nullptr || !NiagaraSystemList.IsValidIndex(entryIndex))
	{
		return;
	}

	const FGameFeatureNiagaraSystemEntry& Entry = NiagaraSystemList[entryIndex]; 
	if (EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved || EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved)
	{
		RemoveNiagaraSystem(Actor, *ActiveData);
	} else if (EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded || EventName == UGameFrameworkComponentManager::NAME_GameActorReady)
	{
		SpawnNiagaraSystem(Actor, Entry, *ActiveData);
	}
}

void UGameFeatureAction_AddNiagara::RemoveNiagaraSystem(AActor* Actor, FPerContextData& ActiveData)
{
	UE_LOG(LogTemp, Display, TEXT("Removing Niagara system"));

	TArray<UNiagaraComponent*> NiagaraComponents = ActiveData.ActiveSystems.FindOrAdd(Actor);
	for (UNiagaraComponent* NiagaraComponent : NiagaraComponents)
	{
		NiagaraComponent->Deactivate();
	}

	ActiveData.ActiveSystems.Remove(Actor);
}

void UGameFeatureAction_AddNiagara::SpawnNiagaraSystem(AActor* Actor, const FGameFeatureNiagaraSystemEntry& Entry,
	FPerContextData& ActiveData)
{
	TObjectPtr<UNiagaraComponent> NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		Entry.NiagaraSystem,
		Actor->GetRootComponent(),
		FName("None"),
		Actor->GetActorLocation(),
		Actor->GetActorRotation(),
		EAttachLocation::KeepWorldPosition,
		true);

	ActiveData.ActiveSystems.FindOrAdd(Actor).Add(NiagaraComponent);
}
