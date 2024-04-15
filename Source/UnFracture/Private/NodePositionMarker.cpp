// Copyright Daniel Sellers 2023


#include "NodePositionMarker.h"

// Sets default values
ANodePositionMarker::ANodePositionMarker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ANodePositionMarker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANodePositionMarker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

