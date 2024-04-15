// Copyright Daniel Sellers 2023


#include "ShipGameModeBase.h"

AShipGameModeBase::AShipGameModeBase()
{
	DeformationVictoryLimit = 0.60;

	bGameOver = false;

	PlayerHasWon = false;
}

void AShipGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACompressableGameActor::StaticClass(), Compressables);

	CollectStressIndicators();

	UE_LOG(LogTemp, Warning, TEXT("Stress Indicators Found: %d"), Indicators.Num());
}

void AShipGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackTime(DeltaTime);
	CheckVictoryConditions();
}

void AShipGameModeBase::CollectStressIndicators()
{
	for (AActor* Compressable : Compressables)
	{
		UActorComponent* Component = Compressable->GetComponentByClass(UStressIndicatorComponent::StaticClass());
		UStressIndicatorComponent* Indicator = Cast<UStressIndicatorComponent>(Component);
		if (Indicator) Indicators.Add(Indicator);
	}
}

bool AShipGameModeBase::CheckDeformationsAcceptable()
{
	float MaxDeformationScale = 0.0;
	float MinDeformationScale = 2;

	for (UStressIndicatorComponent* Indicator : Indicators)
	{
		float DeformationScale = Indicator->GetDeformationScale();
		if (DeformationScale > MaxDeformationScale)
		{
			MaxDeformationScale = DeformationScale;
		}
		if (DeformationScale < MinDeformationScale)
		{
			MinDeformationScale = DeformationScale;
		}
	}

	if (MaxDeformationScale < DeformationVictoryLimit)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void AShipGameModeBase::CheckVictoryConditions()
{
	if (!bGameOver && CheckDeformationsAcceptable() && GameTimeElapsed > 5)
	{
		PlayerHasWon = true;
		GameOver();
	}
}

void AShipGameModeBase::TrackTime(float DeltaTime)
{
	GameTimeElapsed += DeltaTime;
}

void AShipGameModeBase::GameOver()
{
	Super::GameOver();
}