// Copyright Daniel Sellers 2023


#include "SupportBeamPickup.h"
#include "SupportBeamAbility.h"

ASupportBeamPickup::ASupportBeamPickup()
{
	SupportBeamAmmoToSet = 4;
}

void ASupportBeamPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	AAbilityBase* SpawnedAbilityActor = abilityComponent->AddLMBAbility(AbilityClassToGrant);

	ASupportBeamAbility* SpawnedSupportLauncher = Cast<ASupportBeamAbility>(SpawnedAbilityActor);

	if (SpawnedSupportLauncher)
	{
		SpawnedSupportLauncher->SetPushingForceToSet(PushingForceToSet);
		SpawnedSupportLauncher->SetWidthToSet(WidthToSet);
	}

	abilityComponent->AddAmmo(AbilityClassToGrant, SupportBeamAmmoToSet);
}
