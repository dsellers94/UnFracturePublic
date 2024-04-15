// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneAbilityComponent.h"
#include "DroneBase.h"
#include "ItemPickup.generated.h"

UCLASS()
class UNFRACTURE_API AItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemPickup();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int QuantityInStock;

	UFUNCTION()
	void OnOverlapBegin(
		AActor* OverlappedActor,
		AActor* OtherActor);

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
