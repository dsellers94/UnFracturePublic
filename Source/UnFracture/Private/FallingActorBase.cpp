// Copyright Daniel Sellers 2023


#include "FallingActorBase.h"
#include "CompressableGameActor.h"
#include "Kismet/KismetMathLibrary.h"

AFallingActorBase::AFallingActorBase()
{
	Width = 100;

	PrimaryActorTick.bCanEverTick = true;

	OnActorHit.AddDynamic(this, &AFallingActorBase::OnHit);
}

void AFallingActorBase::BeginPlay()
{
	Super::BeginPlay();

	UStaticMeshComponent* FallingStaticMesh = Cast<UStaticMeshComponent>(GetRootComponent());

	float FallingActorMass = FallingStaticMesh->GetMass();

	DownForce = FallingActorMass * 9.8;
}

void AFallingActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


float AFallingActorBase::GetDownForce()
{
	return DownForce;
}

float AFallingActorBase::GetWidth()
{
	return Width;
}

void AFallingActorBase::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	ACompressableGameActor* CompressableHit = Cast<ACompressableGameActor>(OtherActor);

	if (CompressableHit)
	{
		CompressableHit->RegisterFallingActor(GetActorNameOrLabel(), this);
	}
}