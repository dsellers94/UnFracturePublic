// Copyright Daniel Sellers 2023


#include "DroneBase.h"
#include "Kismet/GameplayStatics.h"
#include "DroneAbilityComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ADroneBase::ADroneBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMesh);

	Acceleration = 200.0;

	BoostAcceleration = 500;

	Velocity = FVector(0.0, 0.0, 0.0);

	RMBIsDown = false;

	CameraScrollSpeed = 300.0;

	MaxOrthoWidth = 6000.0;

	World = GetWorld();

}

// Called when the game starts or when spawned
void ADroneBase::BeginPlay()
{
	Super::BeginPlay();

	USceneComponent* Component = GetRootComponent();

	RootPrimComp = Cast<UPrimitiveComponent>(Component);

	AbilityComponent = FindComponentByClass<UDroneAbilityComponent>();

	CameraComponent = FindComponentByClass<UCameraComponent>();
}

// Called every frame
void ADroneBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADroneBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Enhanced Input Logic NOTE: Moved all input binding to DronePlayercontroller but keeping this funciton here as a stub in case I want to move any inputs back here
	//UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

}

void ADroneBase::MoveUp(float inputValue)
{
	float DeltaSeconds = World->GetDeltaSeconds();

	FVector CompVelocity = RootPrimComp->GetComponentVelocity();
	CompVelocity.Z += inputValue * Acceleration * DeltaSeconds;
	RootPrimComp->SetAllPhysicsLinearVelocity(CompVelocity, false);

}

void ADroneBase::MoveRight(float inputValue)
{
	float DeltaSeconds = World->GetDeltaSeconds();
	
	FVector CompVelocity = RootPrimComp->GetComponentVelocity();
	CompVelocity.Y += inputValue * Acceleration * DeltaSeconds;
	RootPrimComp->SetAllPhysicsLinearVelocity(CompVelocity, false);

	//Velocity.Y += inputValue * Acceleration * DeltaSeconds;
}

void ADroneBase::PrimaryAction(bool inputBool)
{
	if (AbilityComponent)
	{
		AbilityComponent->FireAbility(inputBool);
	}
}

void ADroneBase::Boost(bool inputBool)
{
	if (inputBool)
	{
		ReturnAcceleration = Acceleration;
		Acceleration = BoostAcceleration;
	}
	else
	{
		Acceleration = ReturnAcceleration;
	}
}

void ADroneBase::Zoom(float inputValue)
{
	if (RMBIsDown)
	{
		ScrollAbility(inputValue);
	}
	else
	{
		ScrollCamera(inputValue);
	}
}

void ADroneBase::RMBAction(bool inputBool)
{
	RMBIsDown = inputBool;
	if (inputBool)
	{
		OnModeAction.Broadcast();
		RMBDownEvent();
	}
	else
	{
		OnStopModeAction.Broadcast();
		RMBUpEvent();
	}
}

void ADroneBase::Rotate(FVector2D inputVector2D)
{
	FVector NewVector = FVector(0.0, inputVector2D.X, -inputVector2D.Y);
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(FVector(1, 0, 0), NewVector);
	SetActorRotation(NewRotation);
}

void ADroneBase::Continue(bool inputBool)
{
	OnContinueEvent.Broadcast();
}

void ADroneBase::ScrollAbility(float inputValue)
{
	if (AbilityComponent)
	{
		AbilityComponent->ScrollAbility(inputValue);
	}
}

void ADroneBase::ScrollCamera(float inputValue)
{
	if (CameraComponent)
	{
		float OrthoWidth = CameraComponent->OrthoWidth;
		float NewOrthoWidth = OrthoWidth - inputValue * CameraScrollSpeed;
		NewOrthoWidth = UKismetMathLibrary::FClamp(NewOrthoWidth, 300.0, MaxOrthoWidth);
		CameraComponent->SetOrthoWidth(NewOrthoWidth);
	}
}

UStaticMeshComponent* ADroneBase::GetStaticMesh()
{
	return StaticMesh;
}

