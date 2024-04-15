// Copyright Daniel Sellers 2023


#include "ClickAbilityTest.h"

AClickAbilityTest::AClickAbilityTest()
{

}

void AClickAbilityTest::BeginPlay()
{
	Super::BeginPlay();


}

void AClickAbilityTest::Activate()
{
}

void AClickAbilityTest::Deactivate()
{
}

void AClickAbilityTest::PrintTest()
{
	UE_LOG(LogTemp, Warning, TEXT("HE'S FIGHTEN LIKE A MARLIN!!!"));
}

void AClickAbilityTest::Fire(bool inputBool)
{
	if (inputBool)
	{
		PrintTest();
	}
}