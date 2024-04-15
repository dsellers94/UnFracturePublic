// Copyright Daniel Sellers 2023


#include "AbilityBase.h"
#include "DroneAbilityComponent.h"

// Sets default values
AAbilityBase::AAbilityBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AAbilityBase::BeginPlay()
{
	Super::BeginPlay();

}

void AAbilityBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAbilityBase::Activate()
{
	IsActive = true;
}

void AAbilityBase::Deactivate()
{
	IsActive = false;
}

void AAbilityBase::Fire(bool inputBool)
{

}

void AAbilityBase::RegisterOwner()
{
	DroneOwner = Cast<ADroneBase>(GetAttachParentActor());
}

void AAbilityBase::RegisterAbilityComponent(UDroneAbilityComponent* inAbilityComponent)
{
	AbilityComponent = inAbilityComponent;
}


