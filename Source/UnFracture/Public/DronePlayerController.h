// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "DroneBase.h"
#include "DronePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API ADronePlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:

	ADronePlayerController();

	virtual void Tick(float DeltaTime) override;

	ADroneBase* ControlledDrone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool GamePadMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FVector PlayerMouseSeparationVector;

	UFUNCTION(BlueprintImplementableEvent)
	void ContinueEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void PauseEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void UnPauseEvent();

	UFUNCTION(BlueprintCallable)
	void SwitchContext(UInputMappingContext* newInputContext);

	virtual void SetupInputComponent() override;

	void EnhancedMoveUp(const FInputActionValue& Value);

	void EnhancedMoveRight(const FInputActionValue& Value);

	void EnhancedPrimaryAction(const FInputActionValue& Value);

	void EnhancedRMBAction(const FInputActionValue& Value);

	void EnhancedBoost(const FInputActionValue& Value);

	void EnhancedZoom(const FInputActionValue& Value);

	void EnhancedRotate(const FInputActionValue& Value);

	void EnhancedPause(const FInputActionValue& Value);

	void EnhancedContinue(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputMappingContext* inputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputMoveUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputMoveRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputPrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputRMBAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputBoost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputZoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputRotate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputPause;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	UInputAction* inputContinue;
};
