// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "FallingActorBase.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AFallingActorBase : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:

	AFallingActorBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Width;

	UPROPERTY()
	float DownForce;

	UFUNCTION()
	float GetDownForce();

	UFUNCTION()
	float GetWidth();

private:

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

protected:
	virtual void BeginPlay() override;
};
