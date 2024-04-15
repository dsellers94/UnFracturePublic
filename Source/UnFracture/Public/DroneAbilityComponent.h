// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DroneBase.h"
#include "AbilityBase.h"
#include "DroneAbilityComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UNFRACTURE_API UDroneAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDroneAbilityComponent();

	UWorld* World;

	ADroneBase* DroneOwner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<AAbilityBase*> AvailableLMBAbilities;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSubclassOf<AAbilityBase>> ExistingLMBAbilityClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AAbilityBase* ActiveLMBAbility;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int LMBAbilityIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<TSubclassOf<AAbilityBase>, int> AmmoInventoryMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FString, TSubclassOf<AAbilityBase>> NameClassMap;

	UFUNCTION(BlueprintCallable)
	void AddAmmo(TSubclassOf<AAbilityBase> AmmoType, int AmmoCount);

	UFUNCTION(BlueprintCallable)
	int GetAmmoCount(TSubclassOf<AAbilityBase> AmmoType);

	UFUNCTION(BlueprintCallable)
	void SpendAmmo(TSubclassOf<AAbilityBase> AmmoType);

	UFUNCTION(BlueprintCallable)
	void RegisterLMBAbilityClass(TSubclassOf<AAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable)
	AAbilityBase* AddLMBAbility(TSubclassOf<AAbilityBase> AbilityClass);

	UFUNCTION(BlueprintCallable)
	void DeactivateLMBAbility();

	UFUNCTION(BlueprintCallable)
	void ScrollAbility(float inputValue);

	UFUNCTION(BlueprintImplementableEvent)
	void AbilityScrollEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateAmmoEvent();

	UFUNCTION()
	void FireAbility(bool inputBool);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
