#include "Pawns/SamplePawn.h"

#include "Components/CapsuleComponent.h"

ASamplePawn::ASamplePawn()
{
	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	SetRootComponent(CapsuleComponent);
}
