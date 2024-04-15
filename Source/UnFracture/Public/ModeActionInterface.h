// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ModeActionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UModeActionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UNFRACTURE_API IModeActionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION()
	virtual void OnModeAction();

	UFUNCTION()
	virtual void OnStopModeAction();
};
