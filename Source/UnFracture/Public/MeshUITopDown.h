// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MeshUITopDown.generated.h"

UCLASS()
class UNFRACTURE_API AMeshUITopDown : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMeshUITopDown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
