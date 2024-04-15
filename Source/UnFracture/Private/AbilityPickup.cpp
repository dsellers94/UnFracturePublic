// Copyright Daniel Sellers 2023


#include "AbilityPickup.h"

AAbilityPickup::AAbilityPickup()
{
	QuantityInStock = 1;
}

void AAbilityPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	abilityComponent->AddLMBAbility(AbilityClassToGrant);
}