// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "DroneGameModeBase.h"
#include "StressIndicatorComponent.h"
#include "CompressableGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "StabilizeGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AStabilizeGameMode : public ADroneGameModeBase
{
	GENERATED_BODY()

public:

	AStabilizeGameMode();

	virtual void Tick(float DeltaTime) override;

	TArray<UStressIndicatorComponent*> Indicators;

	TArray<AActor*> Compressables;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float GameTimeLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GameTimeElapsed;

	UPROPERTY(EditAnywhere)
	float DeformationVictoryLimit;

	float VictoryConfirmationTime;

	float VictoryTimeCount;

	UFUNCTION()
	void TrackTimeLimit(float DeltaTime);

	UFUNCTION()
	bool CheckDeformationsAcceptable();

	void DeclareVictory();

	void CollectStressIndicators();

	virtual void GameOver() override;
	
protected:

	virtual void BeginPlay() override;
};
