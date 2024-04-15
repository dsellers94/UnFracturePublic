// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "DynamicPushingActor.h"
#include "SupportBeamAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API ASupportBeamAbility : public AAbilityBase
{
	GENERATED_BODY()
	
public:

	ASupportBeamAbility();

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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADynamicPushingActor> SupportBeamClass;

	void SetPushingForceToSet(float pushingForceToSet);

	void SetWidthToSet(float widthToSet);

	virtual void Fire(bool inputBool) override;

;
};
