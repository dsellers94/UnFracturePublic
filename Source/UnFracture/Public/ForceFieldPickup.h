// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityPickup.h"
#include "ForceFieldBase.h"
#include "ForceFieldAbility.h"
#include "ForceFieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AForceFieldPickup : public AAbilityPickup
{
	GENERATED_BODY()

public:

	AForceFieldPickup();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AForceFieldBase> ForceFieldClassToSet;

	UPROPERTY(EditAnywhere)
	float ForceFieldStrengthToSet;

	UPROPERTY(EditAnywhere)
	float EffectiveRangeToSet;

	UPROPERTY(EditAnywhere)
	float MaxStrengthToSet;

	UPROPERTY(EditAnywhere)
	float RampUpTimeToSet;

	UPROPERTY(EditAnywhere)
	bool UseRampUp;

	virtual void GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent) override;

	
};
