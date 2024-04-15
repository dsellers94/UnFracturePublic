// Copyright Daniel Sellers 2023


#include "DronePlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

ADronePlayerController::ADronePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;

	GamePadMode = true;
}

void ADronePlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(inputMappingContext, 0);
	}

	ControlledDrone = Cast<ADroneBase>(GetPawn());

	SetShowMouseCursor(true);
}

void ADronePlayerController::Tick(float DeltaTime)
{
	if (!GamePadMode && ControlledDrone)
	{
		FVector MouseWorldLocation;
		FVector MouseWorldDirection;
		DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);
		FVector DroneLocation = ControlledDrone->GetActorLocation();
		PlayerMouseSeparationVector = FVector(0.0, MouseWorldLocation.Y - DroneLocation.Y, MouseWorldLocation.Z - DroneLocation.Z);
		FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(FVector(1, 0, 0), PlayerMouseSeparationVector);

		ControlledDrone->SetActorRotation(NewRotation);
	}
}

void ADronePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);

	EnhancedInput->BindAction(inputMoveUp, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedMoveUp);
	EnhancedInput->BindAction(inputMoveRight, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedMoveRight);
	EnhancedInput->BindAction(inputRMBAction, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedRMBAction);
	EnhancedInput->BindAction(inputPrimaryAction, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedPrimaryAction);
	EnhancedInput->BindAction(inputBoost, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedBoost);
	EnhancedInput->BindAction(inputZoom, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedZoom);
	EnhancedInput->BindAction(inputRotate, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedRotate);
	EnhancedInput->BindAction(inputPause, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedPause);
	EnhancedInput->BindAction(inputContinue, ETriggerEvent::Triggered, this, &ADronePlayerController::EnhancedContinue);

}

void ADronePlayerController::SwitchContext(UInputMappingContext* newInputContext)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->RemoveMappingContext(inputMappingContext);
		inputMappingContext = newInputContext;
		Subsystem->AddMappingContext(newInputContext, 0);
	}
}

void ADronePlayerController::EnhancedMoveUp(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (ControlledDrone)
	{
		ControlledDrone->MoveUp(AxisValue);
	}
}

void ADronePlayerController::EnhancedMoveRight(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (ControlledDrone)
	{
		ControlledDrone->MoveRight(AxisValue);
	}
}

void ADronePlayerController::EnhancedRMBAction(const FInputActionValue& Value)
{
	const bool BoolValue = Value.Get<bool>();
	if (ControlledDrone)
	{
		ControlledDrone->RMBAction(BoolValue);
	}
}

void ADronePlayerController::EnhancedPrimaryAction(const FInputActionValue& Value)
{
	const bool BoolValue = Value.Get<bool>();
	if (ControlledDrone)
	{
		ControlledDrone->PrimaryAction(BoolValue);
	}
}

void ADronePlayerController::EnhancedBoost(const FInputActionValue& Value)
{
	const bool BoolValue = Value.Get<bool>();
	if (ControlledDrone)
	{
		ControlledDrone->Boost(BoolValue);
	}
}

void ADronePlayerController::EnhancedZoom(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (ControlledDrone)
	{
		ControlledDrone->Zoom(AxisValue);
	}
}

void ADronePlayerController::EnhancedRotate(const FInputActionValue& Value)
{
	const FVector2D AxisValue = Value.Get<FVector2D>();
	if (ControlledDrone)
	{
		ControlledDrone->Rotate(AxisValue);
	}
}

void ADronePlayerController::EnhancedPause(const FInputActionValue& Value)
{
	if (GetWorld()->IsPaused())
	{
		SetPause(false);
		UnPauseEvent();
	}
	else
	{
		SetPause(true);
		PauseEvent();
	}
}

void ADronePlayerController::EnhancedContinue(const FInputActionValue& Value)
{
	const bool inputBool = Value.Get<bool>();
	if (ControlledDrone)
	{
		ControlledDrone->Continue(inputBool);
	}
}