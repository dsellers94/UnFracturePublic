// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RecorderEdSaveGame.h"
#include "RecorderEdBlueprintLibrary.generated.h"


UCLASS()
class LEVELRECORDERED_API URecorderEdBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	static void LoadRecord(
		int SlotIndex,
		TArray<TSubclassOf<AActor>>& outActorClassReferences,
		TArray<FTransform>& outActorTransforms,
		TArray<FTagArray>& outActorTagArrays);

	UFUNCTION(BlueprintCallable)
		static void SaveRecord(
		TArray<TSubclassOf<AActor>> ActorClassReferences,
		TArray<FTransform> ActorTransforms,
		TArray<FTagArray> ActorTagArrays,
		int SlotIndex);

	UFUNCTION(BlueprintCallable)
	static TArray<FTransform> RecordTargetActorTransforms(TArray<TSubclassOf<AActor>> inTargetParentClasses);

	UFUNCTION(BlueprintCallable)
	static TArray<TSubclassOf<AActor>> RecordTargetActorClasses(TArray<TSubclassOf<AActor>> inTargetParentClasses);

	UFUNCTION(BlueprintCallable)
	static TArray<FTagArray> RecordTargetActorTagArrays(TArray<TSubclassOf<AActor>> inTargetParentClasses);

	UFUNCTION(BlueprintCallable)
	static void SpawnActorsFromRecord(
		TArray<TSubclassOf<AActor>> ActorClassReferences, 
		TArray<FTransform> ActorTransforms,
		TArray<FTagArray> ActorTags);

	UFUNCTION(BlueprintCallable)
	static void ClearTargetActors(TArray<TSubclassOf<AActor>> inTargetParentClasses);
};
