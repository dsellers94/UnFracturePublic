// Copyright Daniel Sellers 2023


#include "TriggerSwitch.h"

ATriggerSwitch::ATriggerSwitch()
{
	PrimaryActorTick.bCanEverTick = true;

	OnActorBeginOverlap.AddDynamic(this, &ATriggerSwitch::OnBeginOverlap);
}

void ATriggerSwitch::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATriggerSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATriggerSwitch::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor->ActorHasTag(ObjectiveTag))
	{
		OnGameplayObjective.Broadcast();
	}

	if (OtherActor->ActorHasTag(FailureTag))
	{
		OnGameplayFailure.Broadcast();
	}
}

