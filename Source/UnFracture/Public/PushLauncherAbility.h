// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "DynamicPushingActor.h"
#include "DroneAbilityComponent.h"
#include "PushLauncherAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API APushLauncherAbility : public AAbilityBase
{
	GENERATED_BODY()

public:

	APushLauncherAbility();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MuzzleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LaunchSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PushingVectorToSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PushingForceToSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WidthToSet;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADynamicPushingActor> PushingActorClass;

	UPROPERTY(EditAnywhere)
	bool IsSolidToSet;

	virtual void Fire(bool inputBool) override;

	void SetPushingForceToSet(float pushingForceToSet);

	void SetWidthToSet(float widthToSet);

	void SetIsSolidToSet(bool inIsSolidToSet);

protected:
	virtual void BeginPlay() override;
};
