// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "ItemPickup.h"
#include "AbilityPickup.generated.h"


UCLASS()
class UNFRACTURE_API AAbilityPickup : public AItemPickup
{
	GENERATED_BODY()
	
public: 

	AAbilityPickup();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAbilityBase> AbilityClassToGrant;

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent) override;
};
