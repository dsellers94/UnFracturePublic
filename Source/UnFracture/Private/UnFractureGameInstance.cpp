// Copyright Daniel Sellers 2023


#include "UnFractureGameInstance.h"
#include "Kismet/GameplayStatics.h"

UUnFractureGameInstance::UUnFractureGameInstance()
{
	LoadGameInstance();
}

void UUnFractureGameInstance::SetCurrentGenerationDataFileName(FString fileName)
{
	CurrentGenerationDataFileName = fileName;
}

FString UUnFractureGameInstance::GetCurrentGenerationDataFileName()
{
	return CurrentGenerationDataFileName;
}

void UUnFractureGameInstance::SaveGameInstance()
{
	UInstanceSaveGame* InstanceSaveGame = Cast<UInstanceSaveGame>(UGameplayStatics::CreateSaveGameObject(UInstanceSaveGame::StaticClass()));

	InstanceSaveGame->CurrentGenerationDataFileName = CurrentGenerationDataFileName;

	InstanceSaveGame->LevelUnlockMap = LevelUnlockMap;

	UGameplayStatics::SaveGameToSlot(InstanceSaveGame, TEXT("InstanceSaveSlot"), 0);

	UE_LOG(LogTemp, Warning, TEXT("Game Instance Saved"));
}

void UUnFractureGameInstance::LoadGameInstance()
{
	UInstanceSaveGame* InstanceSaveGame = Cast<UInstanceSaveGame>(UGameplayStatics::CreateSaveGameObject(UInstanceSaveGame::StaticClass()));

	InstanceSaveGame = Cast<UInstanceSaveGame>(UGameplayStatics::LoadGameFromSlot("InstanceSaveSlot", 0));

	if(InstanceSaveGame)
	{
		CurrentGenerationDataFileName = InstanceSaveGame->CurrentGenerationDataFileName;

		LevelUnlockMap = InstanceSaveGame->LevelUnlockMap;
	}

	UE_LOG(LogTemp, Warning, TEXT("GameInstanceLoaded"));
}

TArray<FString> UUnFractureGameInstance::GetUnlockedLevels(TArray<FString> inLevelMasterList)
{
	TArray<FString> UnlockedLevels;

	UnlockedLevels.Add(inLevelMasterList[0]);

	if (!LevelUnlockMap.Contains(inLevelMasterList[0]))
	{
		LevelUnlockMap.Add(inLevelMasterList[0], 1);
	}
	else
	{
		LevelUnlockMap[inLevelMasterList[0]] = 1;
	}

	for (int i = 1; i < inLevelMasterList.Num(); i++)
	{
		if (!LevelUnlockMap.Contains(inLevelMasterList[i]))
		{
			LevelUnlockMap.Add(inLevelMasterList[i], 0);
		}

		if (LevelUnlockMap[inLevelMasterList[i]] == 1 || LevelUnlockMap[inLevelMasterList[i - 1]] == 1)
		{
			UnlockedLevels.Add(inLevelMasterList[i]);
		}
	}

	return UnlockedLevels;
}

void UUnFractureGameInstance::MarkLevelComplete(FString inLevelName)
{
	if (LevelUnlockMap.Contains(inLevelName))
	{
		LevelUnlockMap[inLevelName] = 1;
	}
}
