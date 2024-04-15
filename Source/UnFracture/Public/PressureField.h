// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "ForceFieldBase.h"
#include "PressureField.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API APressureField : public AForceFieldBase
{
	GENERATED_BODY()

public:

	APressureField();

	virtual TArray<int32> GetForceFieldTargetNodes(AActor* EffectedCompressable) override;


	
};

//int HitCount = 0;

//for (int i = 0; i < CompressableEdgeNodes.Num(); i++)
//{
//	FHitResult HitResult;
//	FCollisionQueryParams Params;
//	FCollisionResponseParams ResponseParam;
//	FVector NodeWorldLocation = CompressableNodePositions[CompressableEdgeNodes[i]] + EffectedCompressable->GetActorLocation();
//	bool bDidHit = GetWorld()->LineTraceSingleByChannel(
//		HitResult,
//		GetActorLocation(),
//		NodeWorldLocation,
//		ECollisionChannel::ECC_Visibility,
//		Params,
//		ResponseParam);

//	//DrawDebugLine(GetWorld(), GetActorLocation(), NodeWorldLocation, FColor::Blue, false); 7153955
//	if (bDidHit)
//	{
//		HitCount += 1;
//	}

//}

//UE_LOG(LogTemp, Warning, TEXT("Hit Count: %d"), HitCount);