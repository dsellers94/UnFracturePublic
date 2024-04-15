// Copyright Daniel Sellers 2023


#include "DroneAbilityComponent.h"
#include "Kismet/KismetMathLibrary.h"


UDroneAbilityComponent::UDroneAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	World = GetWorld();

	DroneOwner = Cast<ADroneBase>(GetOwner());

}

void UDroneAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UDroneAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


}

AAbilityBase* UDroneAbilityComponent::AddLMBAbility(TSubclassOf<AAbilityBase> AbilityClass)
{
	if (!ExistingLMBAbilityClasses.Contains(AbilityClass))
	{
		FVector Location = FVector(0.0, 0.0, 0.0);
		FRotator Rotation = FRotator(0.0, 0.0, 0.0);
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		AActor* SpawnedActor = World->SpawnActor<AActor>(AbilityClass, Location, Rotation, SpawnInfo);

		AAbilityBase* SpawnedAbilityActor = Cast<AAbilityBase>(SpawnedActor);

		if (!SpawnedAbilityActor) return nullptr;

		FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(
			EAttachmentRule::KeepRelative,
			EAttachmentRule::KeepRelative,
			EAttachmentRule::KeepWorld,
			true);
		bool DidAttach = SpawnedAbilityActor->AttachToComponent(
			Cast<USceneComponent>(DroneOwner->GetRootComponent()),
			AttachmentTransformRules);

		SpawnedAbilityActor->RegisterOwner();
		SpawnedAbilityActor->Activate();
		SpawnedAbilityActor->RegisterAbilityComponent(this);

		UpdateAmmoEvent();

		RegisterLMBAbilityClass(AbilityClass);

		AvailableLMBAbilities.Add(SpawnedAbilityActor);

		return SpawnedAbilityActor;

	}
	else
	{
		return nullptr;
	}
}

void UDroneAbilityComponent::DeactivateLMBAbility()
{
	if (ActiveLMBAbility)
	{
		ActiveLMBAbility->Deactivate();

		ActiveLMBAbility = nullptr;
	}
}

void UDroneAbilityComponent::RegisterLMBAbilityClass(TSubclassOf<AAbilityBase> AbilityClass)
{
	if (!ExistingLMBAbilityClasses.Contains(AbilityClass))
	{
		ExistingLMBAbilityClasses.Add(AbilityClass);
	}
}

void UDroneAbilityComponent::ScrollAbility(float inputValue)
{
	int inputInt = UKismetMathLibrary::Round64(inputValue);
	int AbilityListLength = AvailableLMBAbilities.Num();

	if (AbilityListLength == 0) return;

	LMBAbilityIndex += inputInt;

	if (LMBAbilityIndex < 0)
	{
		LMBAbilityIndex = AbilityListLength - 1;
	}
	else if (LMBAbilityIndex >= AbilityListLength)
	{
		LMBAbilityIndex = 0;
	}

	DeactivateLMBAbility();
	ActiveLMBAbility = AvailableLMBAbilities[LMBAbilityIndex];
	ActiveLMBAbility->Activate();
	AbilityScrollEvent();
	UpdateAmmoEvent();
}

void UDroneAbilityComponent::FireAbility(bool inputBool)
{
	if (ActiveLMBAbility)
	{
		ActiveLMBAbility->Fire(inputBool);
	}
	UpdateAmmoEvent();
}

void UDroneAbilityComponent::AddAmmo(TSubclassOf<AAbilityBase> AmmoType, int AmmoCount)
{
	if (AmmoInventoryMap.Contains(AmmoType))
	{
		AmmoInventoryMap[AmmoType] += AmmoCount;
	}
	else
	{
		AmmoInventoryMap.Add(AmmoType, AmmoCount);
	}

	UpdateAmmoEvent();
}

int UDroneAbilityComponent::GetAmmoCount(TSubclassOf<AAbilityBase> AmmoType)
{
	if (AmmoInventoryMap.Contains(AmmoType))
	{
		return AmmoInventoryMap[AmmoType];
	}
	else
	{
		return 0;
	}
}

void UDroneAbilityComponent::SpendAmmo(TSubclassOf<AAbilityBase> AmmoType)
{
	if (AmmoInventoryMap.Contains(AmmoType))
	{
		AmmoInventoryMap[AmmoType] -= 1;
	}

	UpdateAmmoEvent();
}

