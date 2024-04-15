// Copyright Daniel Sellers 2023


#include "SupportBeamAbility.h"
#include "SupportBeamPushingActor.h"
#include "DroneAbilityComponent.h"

ASupportBeamAbility::ASupportBeamAbility()
{
	MuzzleOffset = FVector(0.0, 0.0, 200.0);
	MuzzleDistance = 80.0;
	LaunchSpeed = 300.0;
	PushingForceToSet = 800;
	WidthToSet = 240;
}

void ASupportBeamAbility::Fire(bool inputBool)
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
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		//Spawn Support Beam

		AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(SupportBeamClass, Location, Rotation, SpawnInfo);

		ASupportBeamPushingActor* SpawnedSupportBeam = Cast<ASupportBeamPushingActor>(SpawnedActor);

		//Spawn first Pushing Actor

		SpawnedActor = GetWorld()->SpawnActor<AActor>(PushingActorClass, Location, Rotation, SpawnInfo);

		ADynamicPushingActor* SpawnedPushingActor = Cast<ADynamicPushingActor>(SpawnedActor);

		if (SpawnedPushingActor)
		{
			SpawnedPushingActor->SetVelocity(LaunchDirectionNormal * LaunchSpeed);
			SpawnedPushingActor->SetPushingVector(LaunchDirectionNormal * PushingForceToSet);
			SpawnedPushingActor->SetWidth(WidthToSet);
		}

		//Register pushing actor with support beam

		SpawnedSupportBeam->RegisterConstituentActor(SpawnedPushingActor);

		//Launch second pusher in opposite direction

		Location = GetActorLocation() - LaunchDirectionNormal * MuzzleDistance;
		SpawnedActor = GetWorld()->SpawnActor<AActor>(PushingActorClass, Location, Rotation, SpawnInfo);

		ADynamicPushingActor* ReversedPushingActor = Cast<ADynamicPushingActor>(SpawnedActor);

		if (ReversedPushingActor)
		{
			ReversedPushingActor->SetVelocity(-LaunchDirectionNormal * LaunchSpeed);
			ReversedPushingActor->SetPushingVector(-LaunchDirectionNormal * PushingForceToSet);
			ReversedPushingActor->SetWidth(WidthToSet);
		}

		//Register Reversed pusher with support beam

		SpawnedSupportBeam->RegisterConstituentActor(ReversedPushingActor);

		SpawnedSupportBeam->InitializeMesh();
	}
}

void ASupportBeamAbility::SetPushingForceToSet(float pushingForceToSet)
{
	PushingForceToSet = pushingForceToSet;
}

void ASupportBeamAbility::SetWidthToSet(float widthToSet)
{
	WidthToSet = widthToSet;
}