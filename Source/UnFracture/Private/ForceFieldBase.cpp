// Copyright Daniel Sellers 2023

#include "ForceFieldBase.h"
#include "CompressableBase.h"

AForceFieldBase::AForceFieldBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StrengthCoefficient = 1000;

	DirectionCoefficient = 1;

	FalloffParameter = 2;

	EffectiveRange = 5000;

}

void AForceFieldBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AForceFieldBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AForceFieldBase::GetForceFieldStrength()
{
	return StrengthCoefficient;
}

int AForceFieldBase::GetForceFieldRadialDirection()
{
	return DirectionCoefficient;
}

int AForceFieldBase::GetFalloffParameter()
{
	return FalloffParameter;
}

float AForceFieldBase::GetEffectiveRange()
{
	return EffectiveRange;
}

// This is just a proof of concept for having an override function in forcefields that will take the effected compressable base, get it's edge nodes and
// implement it's own method of choosing target edges nodes from among its potential victims. In this case we're just getting the edge nodes and passing them
// back. Not sure yet how we're going to do more nuanced cases like pressure fields, probably with line traces of some kind.
TArray<int32> AForceFieldBase::GetForceFieldTargetNodes(AActor* EffectedCompressable)
{
	ACompressableBase* CompressableActor = Cast<ACompressableBase>(EffectedCompressable);
	TArray<int32> CompressableEdgeNodes = CompressableActor->GetEdgeNodes();

	return CompressableEdgeNodes;
}

void AForceFieldBase::SetForceFieldStrength(float inStrengthCoefficient)
{
	StrengthCoefficient = inStrengthCoefficient;
}

void AForceFieldBase::SetEffectiveRange(float inEffectiveRange)
{
	EffectiveRange = inEffectiveRange;
}