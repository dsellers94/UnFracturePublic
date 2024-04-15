


// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "StabilizeGameMode.h"
#include "TutorialStabilizeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API ATutorialStabilizeGameMode : public AStabilizeGameMode
{
	GENERATED_BODY()

public: 

	ATutorialStabilizeGameMode();

	virtual void Tick(float DeltaTime) override;

	class ADronePlayerController* DroneController;

	UPROPERTY(EditAnywhere)
	bool IsTimeControlled;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsTicking;

	UPROPERTY()
	float TimeSinceLastTutorialPop;

	UPROPERTY(EditAnywhere)
	float TimeBetweenTutorialPops;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int TutorialStringIndex;

	UFUNCTION(BlueprintCallable)
	void TriggerNextTutorial();

	UFUNCTION(BlueprintCallable)
	void DismissTutorial();

	UFUNCTION(BlueprintCallable)
	void SetIsTicking(bool shouldTick);

	UFUNCTION(BlueprintImplementableEvent)
	void TriggerNextTutorialEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void DismissTutorialEvent();

	UFUNCTION()
	void TrackTutorialTime(bool inputIsTicking, float DeltaTime);

	UFUNCTION()
	void OnContinue();

protected:

	virtual void BeginPlay() override;
};
