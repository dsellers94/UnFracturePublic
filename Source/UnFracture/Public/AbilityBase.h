// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DroneBase.h"
#include "AbilityBase.generated.h"

UCLASS()
class UNFRACTURE_API AAbilityBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAbilityBase();

	UPROPERTY()
	ADroneBase* DroneOwner;

	UPROPERTY()
	FVector SpawnLocation;

	UPROPERTY()
	FRotator SpawnRotation;

	UPROPERTY(BlueprintReadOnly)
	class UDroneAbilityComponent* AbilityComponent;

	UPROPERTY()
	bool IsActive;

	UFUNCTION()
	virtual void Activate();

	UFUNCTION()
	virtual void Deactivate();

	UFUNCTION()
	virtual void Fire(bool inputBool);

	UFUNCTION()
	virtual void RegisterOwner();

	UFUNCTION()
	virtual void RegisterAbilityComponent(UDroneAbilityComponent* inAbilityComponent);

protected:
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
