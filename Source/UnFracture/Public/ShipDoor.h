// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipDoor.generated.h"

UCLASS()
class UNFRACTURE_API AShipDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AShipDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector OpeningVector;

	UPROPERTY()
	FVector ClosedLocation;

	UPROPERTY()
	bool IsOpen;

	UFUNCTION(BlueprintCallable)
	void ToggleDoor();

	UFUNCTION()
	FVector GetOpeningVector();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
