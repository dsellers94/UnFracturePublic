// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DoorManagerComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UNFRACTURE_API UDoorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDoorManagerComponent();

	UPROPERTY(EditAnywhere)
	float DeformationUnlockLimit;

	TArray<AActor*> Compressables;

	TArray<FName> KeyNames;

	TMap<FName, TArray<AActor*>> TagCompressableMap;

	TMap<FName, TArray<AActor*>> TagDoorMap;

	float ComponentTimeElapsed;

	UFUNCTION()
	void CheckDoorKeys();

	void CollectDoorKeys();

	void BuildDoorKeyMaps();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
