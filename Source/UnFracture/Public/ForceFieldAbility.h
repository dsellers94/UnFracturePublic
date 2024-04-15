// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "ForceFieldBase.h"
#include "ForceFieldAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AForceFieldAbility : public AAbilityBase
{
	GENERATED_BODY()

public:

	AForceFieldAbility();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AForceFieldBase> ForceFieldClass;

	UPROPERTY()
	AForceFieldBase* ForceField;

	UPROPERTY()
	float ForceFieldStrengthToSet;

	UPROPERTY()
	float EffectiveRangeToSet;

	UPROPERTY()
	float MaxStrength;

	UPROPERTY()
	float RampUpTime;

	UPROPERTY()
	bool UseRampUp;

	bool IsRampingUp;

	float RampUpTimeElapsed;

	UFUNCTION()
	void SetForceFieldClass(TSubclassOf<AForceFieldBase> inForceFieldClass);

	UFUNCTION()
	void SetForceFieldStrengthToSet(float inForceFieldStrengthToSet);

	UFUNCTION()
	void SetEffectiveRangeToSet(float inEffectiveRangeToSet);

	UFUNCTION()
	void SetMaxStrength(float inMaxStrength);

	UFUNCTION()
	void SetRampUpTime(float inRampUpTime);

	UFUNCTION()
	void SetUseRampUp(bool inUseRampUp);

	virtual void Fire(bool inputBool) override;

	virtual void Deactivate() override;

	virtual void Activate() override;

protected:
	virtual void BeginPlay() override;
	
};
