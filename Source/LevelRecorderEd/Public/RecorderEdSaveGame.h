// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RecorderEdSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FTagArray
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY()
		TArray<FName> TagArray;
};

UCLASS()
class LEVELRECORDERED_API URecorderEdSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	URecorderEdSaveGame();

	UPROPERTY()
		TArray<TSubclassOf<AActor>> ActorClassReferences;

	UPROPERTY()
		TArray<FTransform> ActorTransforms;

	UPROPERTY()
		TArray<float> ActorPlasticities;

	UPROPERTY()
		TArray<FTagArray> ActorTagArrays;
};


