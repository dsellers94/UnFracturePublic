// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "InstanceSaveGame.h"
#include "UnFractureGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API UUnFractureGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UUnFractureGameInstance();

	UPROPERTY(EditAnywhere)
	FString CurrentGenerationDataFileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, int> LevelUnlockMap;

	UFUNCTION(BlueprintCallable)
	void SetCurrentGenerationDataFileName(FString fileName);

	UFUNCTION(BlueprintCallable)
	FString GetCurrentGenerationDataFileName();

	UFUNCTION(BlueprintCallable)
	void SaveGameInstance();

	UFUNCTION(BlueprintCallable)
	void LoadGameInstance();

	UFUNCTION(BlueprintCallable)
	TArray<FString> GetUnlockedLevels(TArray<FString> inLevelMasterList);

	UFUNCTION(BlueprintCallable)
	void MarkLevelComplete(FString inLevelName);


};
