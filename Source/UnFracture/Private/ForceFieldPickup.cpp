// Copyright Daniel Sellers 2023


#include "ForceFieldPickup.h"

AForceFieldPickup::AForceFieldPickup()
{
	ForceFieldStrengthToSet = 0.0;
	EffectiveRangeToSet = 50000;
	MaxStrengthToSet = 10000000;
	RampUpTimeToSet = 6.0;
	UseRampUp = false;
}

void AForceFieldPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	AAbilityBase* SpawnedAbilityActor = abilityComponent->AddLMBAbility(AbilityClassToGrant);

	AForceFieldAbility* SpawnedForceFieldAbility = Cast<AForceFieldAbility>(SpawnedAbilityActor);

	if (SpawnedForceFieldAbility)
	{
		SpawnedForceFieldAbility->SetForceFieldClass(ForceFieldClassToSet);
		SpawnedForceFieldAbility->SetForceFieldStrengthToSet(ForceFieldStrengthToSet);
		SpawnedForceFieldAbility->SetEffectiveRangeToSet(EffectiveRangeToSet);
		SpawnedForceFieldAbility->SetMaxStrength(MaxStrengthToSet);
		SpawnedForceFieldAbility->SetRampUpTime(RampUpTimeToSet);
		SpawnedForceFieldAbility->SetUseRampUp(UseRampUp);
	}

}