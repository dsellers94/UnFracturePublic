// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InstanceSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API UInstanceSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	UInstanceSaveGame();

	UPROPERTY(EditAnywhere)
	FString CurrentGenerationDataFileName;

	UPROPERTY(EditAnywhere)
	TMap<FString, int> LevelUnlockMap;


};
