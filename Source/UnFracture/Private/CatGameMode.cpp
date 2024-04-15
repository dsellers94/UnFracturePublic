// Copyright Daniel Sellers 2023 - 2024


#include "CatGameMode.h"
#include "GameplaySwitch/TriggerSwitch.h"

ACatGameMode::ACatGameMode()
{
	VictoryObjectiveCount = 1;

	GameTimeLimit = 150.0;
}

void ACatGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> TriggerSwitchActors;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATriggerSwitch::StaticClass(), TriggerSwitchActors);

	for (AActor* TriggerSwitchActor : TriggerSwitchActors)
	{
		ATriggerSwitch* TriggerSwitch = Cast<ATriggerSwitch>(TriggerSwitchActor);
		TriggerSwitch->OnGameplayFailure.AddDynamic(this, &ACatGameMode::OnGameplayFailure);
	}
}

void ACatGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TrackTimeLimit(DeltaTime);
}

void ACatGameMode::OnGameplayFailure()
{
	PlayerHasWon = false;
	GameOver();
}