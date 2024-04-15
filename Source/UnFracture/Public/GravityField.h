// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "ForceFieldBase.h"
#include "GravityField.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AGravityField : public AForceFieldBase
{
	GENERATED_BODY()
	
public:

	AGravityField();

	virtual TArray<int32> GetForceFieldTargetNodes(AActor* EffectedCompressable) override;
};
