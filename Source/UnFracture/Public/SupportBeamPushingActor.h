// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "DynamicPushingActor.h"
#include "SupportBeamPushingActor.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API ASupportBeamPushingActor : public ADynamicPushingActor
{
	GENERATED_BODY()
	
public:
	ASupportBeamPushingActor();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TArray<ADynamicPushingActor*> ConstituentActors;

	UPROPERTY(EditDefaultsOnly)
	float BeamWidth;

	UPROPERTY()
	bool ConstituentsAreMoving;

	UFUNCTION()
	void RegisterConstituentActor(ADynamicPushingActor* NewActor);

	FVector ConvertWorldVectorToLocal(FVector WorldSpaceVector);

	virtual void InitializeMesh() override;

	virtual void UpdateMesh() override;

	void CalculateBeamMesh();

	UFUNCTION(BlueprintCallable)
	virtual void Disengage() override;

protected:
	virtual void BeginPlay() override;
};
