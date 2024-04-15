// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityPickup.h"
#include "PushLauncherPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API APushLauncherPickup : public AAbilityPickup
{
	GENERATED_BODY()

public: 

	APushLauncherPickup();

	UPROPERTY(EditAnywhere)
	float PushingForceToSet;

	UPROPERTY(EditAnywhere)
	float WidthToSet;

	UPROPERTY(EditAnywhere)
	int PushingAmmoToSet;

	UPROPERTY(EditAnywhere)
	bool IsSolidToSet;

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent) override;
};
