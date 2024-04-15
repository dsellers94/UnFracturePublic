// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "UnFractureGameModeBase.h"
#include "DroneBase.h"
#include "DroneGameModeBase.generated.h"


UCLASS()
class UNFRACTURE_API ADroneGameModeBase : public AUnFractureGameModeBase
{
	GENERATED_BODY()
	
public:
	ADroneGameModeBase();

	virtual void Tick(float DeltaTime) override;
		
	ADroneBase* PlayerDrone;

	UPROPERTY(EditAnywhere)
	float ConstrictedTimeScale;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool PlayerHasWon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bGameOver;

	UFUNCTION()
	virtual void OnModeAction();

	UFUNCTION()
	virtual void OnStopModeAction();

	void ConstrictTime(float constrictedTimeScale);

	void ReleaseTime();

	UFUNCTION()
	void SubscribeToPlayerDrones();

	UFUNCTION(BlueprintCallable)
	virtual void GameOver();

	UFUNCTION(BlueprintImplementableEvent)
	void GameOverEvent();

	UFUNCTION(BlueprintCallable)
	void SaveProgress();

protected:

	virtual void BeginPlay() override;
};
