// Copyright Daniel Sellers 2023


#include "PushLauncherAbility.h"
#include "DroneAbilityComponent.h"

APushLauncherAbility::APushLauncherAbility()
{
	MuzzleOffset = FVector(0.0, 0.0, 200.0);
	MuzzleDistance = 80.0;
	LaunchSpeed = 300.0;
	WidthToSet = 120;
	IsSolidToSet = true;
}

void APushLauncherAbility::BeginPlay()
{
	Super::BeginPlay();
}

void APushLauncherAbility::Fire(bool inputBool)
{
	TSubclassOf<AAbilityBase> ThisClass = this->GetClass();
	int Ammo = AbilityComponent->GetAmmoCount(ThisClass);
	if (DroneOwner && inputBool && Ammo > 0)
	{
		AbilityComponent->SpendAmmo(ThisClass);

		FVector LaunchDirection = DroneOwner->GetActorUpVector();
		FVector LaunchDirectionNormal = LaunchDirection.GetSafeNormal();
		FVector Location = GetActorLocation() + LaunchDirectionNormal * MuzzleDistance;
		FRotator Rotation = FRotator(0.0, 0.0, 0.0);
		FActorSpawnParameters SpawnInfo;
		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(PushingActorClass, Location, Rotation, SpawnInfo);

		ADynamicPushingActor* SpawnedPushingActor = Cast<ADynamicPushingActor>(SpawnedActor);

		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (SpawnedPushingActor)
		{
			SpawnedPushingActor->SetVelocity(LaunchDirectionNormal * LaunchSpeed);
			SpawnedPushingActor->SetPushingVector(LaunchDirectionNormal * PushingForceToSet);
			SpawnedPushingActor->SetWidth(WidthToSet);
			SpawnedPushingActor->SetIsSolid(IsSolidToSet);
		}
	}
}

void APushLauncherAbility::SetPushingForceToSet(float pushingForceToSet)
{
	PushingForceToSet = pushingForceToSet;
}

void APushLauncherAbility::SetWidthToSet(float widthToSet)
{
	WidthToSet = widthToSet;
}

void APushLauncherAbility::SetIsSolidToSet(bool inIsSolidToSet)
{
	IsSolidToSet = inIsSolidToSet;
}
