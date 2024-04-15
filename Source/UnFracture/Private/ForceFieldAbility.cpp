// Copyright Daniel Sellers 2023


#include "ForceFieldAbility.h"
#include "Kismet/KismetMathLibrary.h"

AForceFieldAbility::AForceFieldAbility()
{

}

void AForceFieldAbility::BeginPlay()
{
	Super::BeginPlay();

}

void AForceFieldAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsRampingUp)
	{
		RampUpTimeElapsed += DeltaTime;

		float CurrentStrength = MaxStrength * (RampUpTimeElapsed / RampUpTime);
		CurrentStrength = UKismetMathLibrary::FClamp(CurrentStrength, 0.0, MaxStrength);

		ForceField->SetForceFieldStrength(CurrentStrength);
		
	}
}

void AForceFieldAbility::Fire(bool inputBool)
{
	if (!ForceField) return;

	if (inputBool)
	{
		if (UseRampUp)
		{
			IsRampingUp = true;
		}
		else
		{
			ForceField->SetForceFieldStrength(MaxStrength);
		}
	}
	else
	{
		ForceField->SetForceFieldStrength(0.0);
		RampUpTimeElapsed = 0;
		IsRampingUp = false;
	}
}

void AForceFieldAbility::Activate()
{
	FVector Location = FVector(0.0, 0.0, 0.0);
	FRotator Rotation = FRotator(0.0, 0.0, 0.0);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ForceFieldClass, Location, Rotation, SpawnInfo);

	ForceField = Cast<AForceFieldBase>(SpawnedActor);

	if (!ForceField) return;

	FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepRelative,
		EAttachmentRule::KeepWorld,
		true);

	bool DidAttach = ForceField->AttachToComponent(
		Cast<USceneComponent>(DroneOwner->GetRootComponent()),
		AttachmentTransformRules);

	ForceField->SetForceFieldStrength(ForceFieldStrengthToSet);
	ForceField->SetEffectiveRange(EffectiveRangeToSet);
}

void AForceFieldAbility::Deactivate()
{
	if (ForceField)
	{
		ForceField->Destroy();
	}
}

void AForceFieldAbility::SetForceFieldClass(TSubclassOf<AForceFieldBase> inForceFieldClass)
{
	ForceFieldClass = inForceFieldClass;
}

void AForceFieldAbility::SetForceFieldStrengthToSet(float inForceFieldStrengthToSet)
{
	ForceFieldStrengthToSet = inForceFieldStrengthToSet;
}

void AForceFieldAbility::SetEffectiveRangeToSet(float inEffectiveRangeToSet)
{
	EffectiveRangeToSet = inEffectiveRangeToSet;
}

void AForceFieldAbility::SetMaxStrength(float inMaxStrength)
{
	MaxStrength = inMaxStrength;
}

void AForceFieldAbility::SetRampUpTime(float inRampUpTime)
{
	RampUpTime = inRampUpTime;
}

void AForceFieldAbility::SetUseRampUp(bool inUseRampUp)
{
	UseRampUp = inUseRampUp;
}
