// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "GrabberAbility.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AGrabberAbility : public AAbilityBase
{
	GENERATED_BODY()
	
public:

	AGrabberAbility();

	virtual void Fire(bool inputBool) override;

	virtual void Deactivate() override;

	virtual void Activate() override;

	TArray<AActor*> OverlappedActors;

	AActor* AttachedActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AAbilityBase> RecycleClass;
};
