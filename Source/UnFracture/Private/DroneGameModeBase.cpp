// Copyright Daniel Sellers 2023


#include "DroneGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UnFractureGameInstance.h"

ADroneGameModeBase::ADroneGameModeBase()
{
	ConstrictedTimeScale = 0.2;

	PlayerDrone;

	PlayerHasWon = false;

	bGameOver = false;
}

void ADroneGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SubscribeToPlayerDrones();
}

void ADroneGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADroneGameModeBase::ConstrictTime(float constrictedTimeScale)
{
	WorldSettings->SetTimeDilation(constrictedTimeScale);
}

void ADroneGameModeBase::ReleaseTime()
{
	WorldSettings->SetTimeDilation(1.0);
}

void ADroneGameModeBase::OnModeAction()
{
	ConstrictTime(ConstrictedTimeScale);
}

void ADroneGameModeBase::OnStopModeAction()
{
	ReleaseTime();
}

void ADroneGameModeBase::GameOver()
{
	if (!bGameOver)
	{
		bGameOver = true;
		GameOverEvent();

		if (PlayerHasWon)
		{
			SaveProgress();
		}
	}
}

void ADroneGameModeBase::SaveProgress()
{
	UUnFractureGameInstance* UnFractureGameInstance = Cast<UUnFractureGameInstance>(GetGameInstance());

	if (UnFractureGameInstance)
	{
		UnFractureGameInstance->MarkLevelComplete(UGameplayStatics::GetCurrentLevelName(GetWorld()));
		UnFractureGameInstance->SaveGameInstance();
	}
}


void ADroneGameModeBase::SubscribeToPlayerDrones()
{
	PlayerDrone = Cast<ADroneBase>(World->GetFirstPlayerController()->GetPawn());

	if (PlayerDrone)
	{
		PlayerDrone->OnModeAction.AddDynamic(this, &ADroneGameModeBase::OnModeAction);
		PlayerDrone->OnStopModeAction.AddDynamic(this, &ADroneGameModeBase::OnStopModeAction);
	}
}
