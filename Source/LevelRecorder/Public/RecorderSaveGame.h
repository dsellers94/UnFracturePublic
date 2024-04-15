// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RecorderSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class LEVELRECORDER_API URecorderSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	URecorderSaveGame();

	UPROPERTY()
		TArray<TSubclassOf<AActor>> ActorClassReferences;

	UPROPERTY()
		TArray<FTransform> ActorTransforms;
	
};
