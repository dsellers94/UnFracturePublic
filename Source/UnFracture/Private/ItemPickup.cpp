// Copyright Daniel Sellers 2023


#include "ItemPickup.h"

// Sets default values
AItemPickup::AItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
    OnActorBeginOverlap.AddDynamic(this, &AItemPickup::OnOverlapBegin);
}

// Called every frame
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemPickup::OnOverlapBegin(
	AActor* OverlappedActor,
	AActor* OtherActor)
{
	if (QuantityInStock <= 0) return;

	ADroneBase* OverlappedDrone = Cast<ADroneBase>(OtherActor);

	if (OverlappedDrone)
	{
		UDroneAbilityComponent* AbilityComponent = OverlappedDrone->FindComponentByClass<UDroneAbilityComponent>();

		if (AbilityComponent)
		{
			GiftToAbilityComponent(AbilityComponent);
			QuantityInStock -= 1;
		}
	}

	if (QuantityInStock <= 0) Destroy();
}

void AItemPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	UE_LOG(LogTemp, Warning, TEXT("I'M GIFTING!"));
}
