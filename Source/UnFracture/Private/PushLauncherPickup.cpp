// Copyright Daniel Sellers 2023


#include "PushLauncherPickup.h"
#include "PushLauncherAbility.h"

APushLauncherPickup::APushLauncherPickup()
{
	WidthToSet = 120;
	PushingForceToSet = 200;
	PushingAmmoToSet = 10;
	IsSolidToSet = true;
}

void APushLauncherPickup::GiftToAbilityComponent(UDroneAbilityComponent* abilityComponent)
{
	AAbilityBase* SpawnedAbilityActor = abilityComponent->AddLMBAbility(AbilityClassToGrant);

	APushLauncherAbility* SpawnedPushLauncher = Cast<APushLauncherAbility>(SpawnedAbilityActor);

	if (SpawnedPushLauncher)
	{
		SpawnedPushLauncher->SetPushingForceToSet(PushingForceToSet);
		SpawnedPushLauncher->SetWidthToSet(WidthToSet);
		SpawnedPushLauncher->SetIsSolidToSet(IsSolidToSet);
	}

	abilityComponent->AddAmmo(AbilityClassToGrant, PushingAmmoToSet);

}
