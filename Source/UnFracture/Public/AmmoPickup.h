// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "ItemPickup.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AAmmoPickup : public AItemPickup
{
	GENERATED_BODY()
	
public:
	
	AAmmoPickup();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int AmmoToGrant;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AAbilityBase> AmmoTypeToGrant;

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent) override;
};
