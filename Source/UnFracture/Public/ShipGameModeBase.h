// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "DroneGameModeBase.h"
#include "StressIndicatorComponent.h"
#include "CompressableGameActor.h"
#include "Kismet/GameplayStatics.h"
#include "ShipGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AShipGameModeBase : public ADroneGameModeBase
{
	GENERATED_BODY()

public:

	AShipGameModeBase();

	virtual void Tick(float DeltaTime) override;

	TArray<UStressIndicatorComponent*> Indicators;

	TArray<AActor*> Compressables;

	UPROPERTY(EditAnywhere)
		float DeformationVictoryLimit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float GameTimeElapsed;

	UFUNCTION()
		void TrackTime(float DeltaTime);

	UFUNCTION()
		bool CheckDeformationsAcceptable();

	UFUNCTION()
		void CheckVictoryConditions();

	void CollectStressIndicators();

	virtual void GameOver() override;
	
protected:

	virtual void BeginPlay() override;


};
