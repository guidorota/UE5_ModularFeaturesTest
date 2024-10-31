// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"
#include "GameFeatureAction_AddNiagara.generated.h"

struct FGameFeatureStateChangeContext;
struct FComponentRequestHandle;
class FDelegateHandle;
class UNiagaraSystem;
class UNiagaraComponent;

USTRUCT()
struct FGameFeatureNiagaraSystemEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TSoftClassPtr<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, Category = "Niagara")
	TObjectPtr<UNiagaraSystem> NiagaraSystem;
};

UCLASS(meta = (DisplayName = "Add Niagara System"))
class UGameFeatureAction_AddNiagara final : public UGameFeatureAction
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, Category = "Niagara")
	TArray<FGameFeatureNiagaraSystemEntry> NiagaraSystemList;
	
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

private:

	struct FPerContextData
	{
		TMap<AActor*, TArray<UNiagaraComponent*>> ActiveSystems;
		TArray<TSharedPtr<FComponentRequestHandle>> ComponentRequests;
	};
	
	void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);
	
	void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext);

	void HandleActorExtension(AActor* Actor, FName EventName, int32 entryIndex, FGameFeatureStateChangeContext ChangeContext);

	void RemoveNiagaraSystem(AActor* Actor, FPerContextData& ActiveData);

	void SpawnNiagaraSystem(AActor* Actor, const FGameFeatureNiagaraSystemEntry& Entry, FPerContextData& ActiveData);
	
	TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;
};
