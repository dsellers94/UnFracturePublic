// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/GameModeBase.h"
#include "Camera/CameraComponent.h"
#include "DroneBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContinueEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FModeAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStopModeAction);

UCLASS()
class UNFRACTURE_API ADroneBase : public APawn
{
	GENERATED_BODY()

public:
	ADroneBase();

	UPROPERTY()
	FContinueEvent OnContinueEvent;

	UPROPERTY()
	FModeAction OnModeAction;

	UPROPERTY()
	FStopModeAction OnStopModeAction;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	UPrimitiveComponent* RootPrimComp;

	UWorld* World;

	UFUNCTION()
	UStaticMeshComponent* GetStaticMesh();

	class UDroneAbilityComponent* AbilityComponent;

	class UCameraComponent* CameraComponent;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere)
	float Acceleration;

	UPROPERTY(EditAnywhere)
	float BoostAcceleration;

	UPROPERTY()
	float ReturnAcceleration;

	UPROPERTY(EditAnywhere)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraScrollSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxOrthoWidth;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool RMBIsDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector PlayerMouseSeparationVector;

	void MoveUp(float inputValue);

	void MoveRight(float inputValue);

	void RMBAction(bool inputBool);

	void PrimaryAction(bool inputBool);

	void Boost(bool inputBool);

	void Zoom(float inputValue);

	void Rotate(FVector2D inputVector2D);

	UFUNCTION(BlueprintImplementableEvent)
	void RMBDownEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void RMBUpEvent();

	void ScrollAbility(float inputValue);

	void ScrollCamera(float inputValue);

	UFUNCTION()
	void Continue(bool inputBool);

};

	//PlayerInputComponent->BindAxis("MoveRight", this, &ADroneBase::MoveRight);
	//PlayerInputComponent->BindAxis("MoveUp", this, &ADroneBase::MoveUp);
	//PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &ADroneBase::OnRightClickDown);
	//PlayerInputComponent->BindAction("RightClick", IE_Released, this, &ADroneBase::OnRightClickUp);
	//PlayerInputComponent->BindAction("Boost", IE_Pressed, this, &ADroneBase::OnBoostDown);
	//PlayerInputComponent->BindAction("Boost", IE_Released, this, &ADroneBase::OnBoostUp);

	//APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
	//UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	//if (PlayerController)
	//{
	//	if (Subsystem)
	//	{
	//		Subsystem->AddMappingContext(inputMappingContext, 0);
	//	}
	//}

	//EnhancedInput->BindAction(inputMoveUp, ETriggerEvent::Triggered, this, &ADroneBase::EnhancedInputMoveUp);
	//EnhancedInput->BindAction(inputMoveRight, ETriggerEvent::Triggered, this, &ADroneBase::EnhancedInputMoveRight);
	//EnhancedInput->BindAction(inputBoost, ETriggerEvent::Triggered, this, &ADroneBase::EnhancedInputBoost);
	//EnhancedInput->BindAction(inputRMBAction, ETriggerEvent::Triggered, this, &ADroneBase::EnhancedRMBAction);
	//EnhancedInput->BindAction(inputZoom, ETriggerEvent::Triggered, this, &ADroneBase::EnhancedZoom);

	//void EnhancedInputMoveUp(const FInputActionValue& Value);

	//void EnhancedInputMoveRight(const FInputActionValue& Value);

	//void EnhancedInputPrimaryAction(const FInputActionValue& Value);

	//void EnhancedInputBoost(const FInputActionValue& Value);

	//void EnhancedRMBAction(const FInputActionValue& Value);

	//void EnhancedZoom(const FInputActionValue& Value);

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputMappingContext* inputMappingContext;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputMoveUp;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputMoveRight;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputPrimaryAction;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputRMBAction;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputBoost;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enhanced Input")
	//UInputAction* inputZoom;

//void ADroneBase::EnhancedInputMoveUp(const FInputActionValue& Value)
//{
//	const float AxisValue = Value.Get<float>();
//	float DeltaSeconds = World->GetDeltaSeconds();
//	Velocity.Z += AxisValue * Acceleration * DeltaSeconds;
//}
//
//void ADroneBase::EnhancedInputMoveRight(const FInputActionValue& Value)
//{
//	const float AxisValue = Value.Get<float>();
//	float DeltaSeconds = World->GetDeltaSeconds();
//	Velocity.Y += AxisValue * Acceleration * DeltaSeconds;
//}
//
//void ADroneBase::EnhancedInputPrimaryAction(const FInputActionValue& Value)
//{
//
//}
//
//void ADroneBase::EnhancedInputBoost(const FInputActionValue& Value)
//{
//	const bool BoolValue = Value.Get<bool>();
//	if (BoolValue)
//	{
//		ReturnAcceleration = Acceleration;
//		Acceleration = BoostAcceleration;
//	}
//	else
//	{
//		Acceleration = ReturnAcceleration;
//	}
//}
//
//void ADroneBase::EnhancedRMBAction(const FInputActionValue& Value)
//{
//	const bool BoolValue = Value.Get<bool>();
//	if (BoolValue)
//	{
//		IModeActionInterface* GameModeInterface = Cast<IModeActionInterface>(GameMode);
//		if (GameModeInterface)
//		{
//			GameModeInterface->OnModeAction(this);
//		}
//		RMBDownEvent();
//	}
//	else
//	{
//		IModeActionInterface* GameModeInterface = Cast<IModeActionInterface>(GameMode);
//		if (GameModeInterface)
//		{
//			GameModeInterface->OnStopModeAction(this);
//		}
//		RMBUpEvent();
//	}
//}
//
//void ADroneBase::EnhancedZoom(const FInputActionValue& Value)
//{
//	const float AxisValue = Value.Get<float>();
//}

//void ADroneBase::UpdateLocation(float DeltaTime)
//{
//	FHitResult Hit;
//	bool DidMove = SetActorLocation(GetActorLocation() + Velocity * DeltaTime, true, &Hit);
//	if (!DidMove)
//	{
//		//Rebound(Hit.ImpactPoint);
//	}
//}

//void ADroneBase::Rebound(FVector ImpactPoint)
//{
//	float Speed = Velocity.Length();
//	FVector Location = GetActorLocation();
//	FVector SeparationVector = Location - ImpactPoint;
//	FVector SeparationNormal = SeparationVector / SeparationVector.Length();
//	float VectorDot = (SeparationVector.Y * Velocity.Y + SeparationVector.Z * Velocity.Z) / (SeparationVector.Length() * Velocity.Length());
//
//	Velocity += -2 * SeparationNormal * Speed * VectorDot;
//	Velocity.X = 0.0;
//
//	UE_LOG(LogTemp, Warning, TEXT("Velocity: %s"), *Velocity.ToString());
//}

//UFUNCTION()
//void UpdateLocation(float DeltaTime);
//
//UFUNCTION()
//void Rebound(FVector ImpactPoint);