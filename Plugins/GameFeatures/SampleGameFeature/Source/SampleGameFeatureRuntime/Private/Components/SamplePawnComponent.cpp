#include "Components/SamplePawnComponent.h"

#include "Components/StaticMeshComponent.h"

USamplePawnComponent::USamplePawnComponent(const FObjectInitializer& ObjectInitializer): UPawnComponent(ObjectInitializer)
{
	
}

void USamplePawnComponent::BeginPlay()
{
	TObjectPtr<AActor> Owner = GetOwner();
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent = NewObject<UStaticMeshComponent>(Owner);
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	StaticMeshComponent->RegisterComponent();
	StaticMeshComponent->AttachToComponent(Owner->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	
	Super::BeginPlay();
}
