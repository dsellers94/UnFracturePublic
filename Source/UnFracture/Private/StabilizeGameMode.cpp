// Copyright Daniel Sellers 2023


#include "StabilizeGameMode.h"

AStabilizeGameMode::AStabilizeGameMode()
{
	GameTimeLimit = 30.0;

	DeformationVictoryLimit = 0.30;

	VictoryConfirmationTime = 1.0;

	VictoryTimeCount = 0;

	bGameOver = false;
}

void AStabilizeGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACompressableGameActor::StaticClass(), Compressables);

	CollectStressIndicators();

	UE_LOG(LogTemp, Warning, TEXT("Stress Indicators Found: %d"), Indicators.Num());
}

void AStabilizeGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackTimeLimit(DeltaTime);
}

void AStabilizeGameMode::CollectStressIndicators()
{
	for (AActor* Compressable : Compressables)
	{
		UActorComponent* Component = Compressable->GetComponentByClass(UStressIndicatorComponent::StaticClass());
		UStressIndicatorComponent* Indicator = Cast<UStressIndicatorComponent>(Component);
		if (Indicator) Indicators.Add(Indicator);
	}
}

void AStabilizeGameMode::TrackTimeLimit(float DeltaTime)
{
	GameTimeElapsed += DeltaTime;

	if (!CheckDeformationsAcceptable())
	{
		VictoryTimeCount = 0;
	}

	if (CheckDeformationsAcceptable() && GameTimeElapsed > 5 && !bGameOver)
	{
		VictoryTimeCount += DeltaTime;

		if (VictoryTimeCount > VictoryConfirmationTime) DeclareVictory();
	} 
	else if (GameTimeElapsed >= GameTimeLimit && !bGameOver)
	{
		UE_LOG(LogTemp, Warning, TEXT("GAME OVER!!! GAME! FUCKING! OVER!!!"));
		UE_LOG(LogTemp, Warning, TEXT("DEFEAT!!!"));
		GameOver();
	}
}

bool AStabilizeGameMode::CheckDeformationsAcceptable()
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

void AStabilizeGameMode::DeclareVictory()
{
	UE_LOG(LogTemp, Warning, TEXT("VICTORY!!!"));
	PlayerHasWon = true;
	GameOver();
}

void AStabilizeGameMode::GameOver()
{
	Super::GameOver();
}