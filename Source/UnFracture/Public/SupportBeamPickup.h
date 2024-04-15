// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityPickup.h"
#include "SupportBeamPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API ASupportBeamPickup : public AAbilityPickup
{
	GENERATED_BODY()
	
public:

	ASupportBeamPickup();

	UPROPERTY(EditAnywhere)
	float PushingForceToSet;

	UPROPERTY(EditAnywhere)
	float WidthToSet;

	UPROPERTY(EditAnywhere)
	int SupportBeamAmmoToSet;

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent) override;
};
