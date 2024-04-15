// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Recorder.generated.h"

UCLASS()
class LEVELRECORDER_API ARecorder : public AActor
{
	GENERATED_BODY()
	
public:	
	ARecorder();

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AActor>> TargetParentClasses;

	UPROPERTY()
	TArray<TSubclassOf<AActor>> ActorClassReferences;

	UPROPERTY()
	TArray<FTransform> ActorTransforms;

	UPROPERTY()
	int ActiveSaveSlot;

	UFUNCTION(BlueprintCallable)
	void RecordTargetActorTransforms();

	UFUNCTION(BlueprintCallable)
	void SaveRecord(int SlotIndex);

	UFUNCTION(BlueprintCallable)
	void LoadRecord(int SlotIndex);

	UFUNCTION(BlueprintCallable)
	void PrintCurrentRecord();

	UFUNCTION(BlueprintCallable)
	void SpawnActorsFromRecord();

	UFUNCTION(BlueprintCallable)
	void SetActiveSaveSlot(int SlotIndex);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};

USTRUCT()
struct FClassTransformPair
{
	GENERATED_BODY()

public:

	TSubclassOf<AActor*> ActorClass;

	FTransform Transform;

};
