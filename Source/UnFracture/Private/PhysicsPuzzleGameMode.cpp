// Copyright Daniel Sellers 2023


#include "PhysicsPuzzleGameMode.h"
#include "GameplaySwitch/TriggerSwitch.h"

APhysicsPuzzleGameMode::APhysicsPuzzleGameMode()
{
	VictoryObjectiveCount = 3;

	GameTimeLimit = 120.0;
}

void APhysicsPuzzleGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> TriggerSwitchActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATriggerSwitch::StaticClass(), TriggerSwitchActors);

	for (AActor* TriggerSwitchActor : TriggerSwitchActors)
	{
		ATriggerSwitch* TriggerSwitch = Cast<ATriggerSwitch>(TriggerSwitchActor);
		TriggerSwitch->OnGameplayObjective.AddDynamic(this, &APhysicsPuzzleGameMode::OnGameplayObjective);
	}
}

void APhysicsPuzzleGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APhysicsPuzzleGameMode::OnGameplayObjective()
{
	RecordObjective();
}

void APhysicsPuzzleGameMode::RecordObjective()
{
	ObjectiveCount++;
	if (CheckVictoryConditions())
	{
		PlayerHasWon = true;
		GameOver();

	}
	UE_LOG(LogTemp, Warning, TEXT("ObjectiveCount: %d"), ObjectiveCount);
}

bool APhysicsPuzzleGameMode::CheckVictoryConditions()
{
	return ObjectiveCount >= VictoryObjectiveCount;
}


void APhysicsPuzzleGameMode::TrackTimeLimit(float DeltaTime)
{
	GameTimeElapsed += DeltaTime;

	if (GameTimeElapsed > GameTimeLimit)
	{
		if (CheckVictoryConditions())
		{
			PlayerHasWon = true;
		}
		else
		{
			PlayerHasWon = false;
		}

		GameOver();
	}
}

void APhysicsPuzzleGameMode::GameOver()
{
	Super::GameOver();
}