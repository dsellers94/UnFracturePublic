// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase.h"
#include "ClickAbilityTest.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AClickAbilityTest : public AAbilityBase
{
	GENERATED_BODY()

public:

	AClickAbilityTest();

	virtual void Activate() override;

	virtual void Deactivate() override;

	void PrintTest();

	virtual void Fire(bool inputBool) override;
	
protected:
	virtual void BeginPlay() override;
};
