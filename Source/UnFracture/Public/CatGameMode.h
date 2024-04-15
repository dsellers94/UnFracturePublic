// Copyright Daniel Sellers 2023 - 2024

#pragma once

#include "CoreMinimal.h"
#include "DroneGameModeBase.h"
#include "PhysicsPuzzleGameMode.h"
#include "CatGameMode.generated.h"


UCLASS()
class UNFRACTURE_API ACatGameMode : public APhysicsPuzzleGameMode
{
	GENERATED_BODY()
	
public:

	ACatGameMode();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnGameplayFailure();

protected:

	virtual void BeginPlay() override;
};
