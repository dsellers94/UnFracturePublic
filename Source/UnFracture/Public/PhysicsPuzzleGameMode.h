// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "DroneGameModeBase.h"
#include "PhysicsPuzzleGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API APhysicsPuzzleGameMode : public ADroneGameModeBase
{
	GENERATED_BODY()
	
public:

	APhysicsPuzzleGameMode();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	int ObjectiveCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GameTimeLimit;

	UPROPERTY(BlueprintReadOnly)
	float GameTimeElapsed;

	UPROPERTY()
	int VictoryObjectiveCount;

	UFUNCTION()
	void OnGameplayObjective();

	UFUNCTION()
	void RecordObjective();

	UFUNCTION()
	bool CheckVictoryConditions();

	UFUNCTION()
	void TrackTimeLimit(float DeltaTime);

	virtual void GameOver() override;

protected:

	virtual void BeginPlay() override;
};
