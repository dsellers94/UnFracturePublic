// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "AbilityToggleTest.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AAbilityToggleTest : public AAbilityBase
{
	GENERATED_BODY()
	
public:

	AAbilityToggleTest();

	virtual void Activate() override;

	virtual void Deactivate() override;

	void PrintTest();

protected:
	virtual void BeginPlay() override;
};
