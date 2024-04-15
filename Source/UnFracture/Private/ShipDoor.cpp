// Copyright Daniel Sellers 2023


#include "ShipDoor.h"
#include "GameplaySwitch/TriggerSwitch.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AShipDoor::AShipDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	IsOpen = false;

	OpeningVector = FVector(0.0, 0.0, 500.0);

}

// Called when the game starts or when spawned
void AShipDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedLocation = GetActorLocation();
}

// Called every frame
void AShipDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShipDoor::ToggleDoor()
{
	if (IsOpen)
	{
		IsOpen = false;
		SetActorLocation(ClosedLocation);
	}
	else
	{
		IsOpen = true;
		SetActorLocation(GetActorLocation() + OpeningVector, false);
	}
}

FVector AShipDoor::GetOpeningVector()
{
	return OpeningVector;
}

