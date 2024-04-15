// Copyright Daniel Sellers 2023

#include "TutorialStabilizeGameMode.h"
#include "DronePlayerController.h"

ATutorialStabilizeGameMode::ATutorialStabilizeGameMode()
{
	TutorialStringIndex = 0;

	TimeSinceLastTutorialPop = 0.0;

	TimeBetweenTutorialPops = 10.0;

	IsTicking = true;

	IsTimeControlled = true;
}

void ATutorialStabilizeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (PlayerDrone) PlayerDrone->OnContinueEvent.AddDynamic(this, &ATutorialStabilizeGameMode::OnContinue);
}

void ATutorialStabilizeGameMode::Tick(float DeltaTime)
{
	if (IsTimeControlled)
	{
		TrackTutorialTime(IsTicking, DeltaTime);
	}
}

void ATutorialStabilizeGameMode::TrackTutorialTime(bool isTicking, float DeltaTime)
{
	if (isTicking)
	{
		TimeSinceLastTutorialPop += DeltaTime;
	}

	if (TimeSinceLastTutorialPop > TimeBetweenTutorialPops)
	{
		TimeSinceLastTutorialPop = 0.0;
		//SetIsTicking(false);
		TriggerNextTutorial();
	}

}

void ATutorialStabilizeGameMode::TriggerNextTutorial()
{
	TriggerNextTutorialEvent();
	TutorialStringIndex += 1;

}

void ATutorialStabilizeGameMode::DismissTutorial()
{
	DismissTutorialEvent();
	//SetIsTicking(true);

}

void ATutorialStabilizeGameMode::SetIsTicking(bool shouldTick)
{
	IsTicking = shouldTick;
}

void ATutorialStabilizeGameMode::OnContinue()
{
	DismissTutorial();
}