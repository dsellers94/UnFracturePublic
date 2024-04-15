// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodePositionMarker.generated.h"

UCLASS()
class UNFRACTURE_API ANodePositionMarker : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANodePositionMarker();

	UPROPERTY(EditAnywhere)
	bool IsOriginMarker;

	UPROPERTY(EditAnywhere)
	bool IsCornerNodeMarker;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
