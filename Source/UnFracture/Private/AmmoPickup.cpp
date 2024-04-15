// Copyright Daniel Sellers 2023


#include "AmmoPickup.h"

AAmmoPickup::AAmmoPickup()
{
	QuantityInStock = 1;
	AmmoToGrant = 3;
}

void AAmmoPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	abilityComponent->AddAmmo(AmmoTypeToGrant, AmmoToGrant);
}