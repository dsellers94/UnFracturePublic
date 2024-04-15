// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ForceFieldBase.generated.h"

UCLASS()
class UNFRACTURE_API AForceFieldBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AForceFieldBase();

	UPROPERTY(EditAnywhere)
	float StrengthCoefficient;

	// Should be only 1 for away from source or -1 for toward source
	UPROPERTY(EditAnywhere)
		int DirectionCoefficient;

	UPROPERTY(EditAnywhere)
		int FalloffParameter;

	UPROPERTY(EditAnywhere)
		float EffectiveRange;

	UFUNCTION()
		float GetForceFieldStrength();

	UFUNCTION()
		int GetForceFieldRadialDirection();

	UFUNCTION()
		int GetFalloffParameter();

	UFUNCTION()
		float GetEffectiveRange();

	UFUNCTION()
		virtual TArray<int32> GetForceFieldTargetNodes(AActor* EffectedCompressable);

	UFUNCTION()
		void SetForceFieldStrength(float inStrengthCoefficient);

	UFUNCTION()
		void SetEffectiveRange(float inEffectiveRange);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
