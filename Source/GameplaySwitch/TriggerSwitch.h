// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TriggerSwitch.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayObjective);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameplayFailure);

UCLASS()
class GAMEPLAYSWITCH_API ATriggerSwitch : public AActor
{
	GENERATED_BODY()
	
public:	
	ATriggerSwitch();

	UPROPERTY()
	FGameplayObjective OnGameplayObjective;

	UPROPERTY()
	FGameplayObjective OnGameplayFailure;

	UPROPERTY(EditAnywhere)
	FName ObjectiveTag;

	UPROPERTY(EditAnywhere)
	FName FailureTag;

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
