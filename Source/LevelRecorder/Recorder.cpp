// Copyright Daniel Sellers 2023


#include "Recorder.h"
#include "Kismet/GameplayStatics.h"
#include "RecorderSaveGame.h"

ARecorder::ARecorder()
{
	PrimaryActorTick.bCanEverTick = true;

} 

void ARecorder::BeginPlay()
{
	Super::BeginPlay();
	
}

void ARecorder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARecorder::RecordTargetActorTransforms()
{
	for (TSubclassOf<AActor> TargetClass : TargetParentClasses)
	{
		TArray<AActor*> TargetClassMembers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), TargetClass, TargetClassMembers);

		for (AActor* TargetActor : TargetClassMembers)
		{
			TSubclassOf<AActor> TargetActorClass = TargetActor->GetClass();
			FTransform TargetActorTransform = TargetActor->GetTransform();

			ActorClassReferences.Add(TargetActorClass);
			ActorTransforms.Add(TargetActorTransform);
		}
	}
}

void ARecorder::SaveRecord(int SlotIndex)
{
	URecorderSaveGame* RecorderSaveGame = Cast<URecorderSaveGame>(UGameplayStatics::CreateSaveGameObject(URecorderSaveGame::StaticClass()));

	RecorderSaveGame->ActorClassReferences = ActorClassReferences;

	RecorderSaveGame->ActorTransforms = ActorTransforms;

	UGameplayStatics::SaveGameToSlot(RecorderSaveGame, TEXT("RecorderSaveSlot"), SlotIndex);

	UE_LOG(LogTemp, Warning, TEXT("Actor Record Saved"));
}

void ARecorder::LoadRecord(int SlotIndex)
{
	URecorderSaveGame* RecorderSaveGame = Cast<URecorderSaveGame>(UGameplayStatics::CreateSaveGameObject(URecorderSaveGame::StaticClass()));

	RecorderSaveGame = Cast<URecorderSaveGame>(UGameplayStatics::LoadGameFromSlot("RecorderSaveSlot", SlotIndex));

	if (RecorderSaveGame)
	{
		ActorClassReferences = RecorderSaveGame->ActorClassReferences;

		ActorTransforms = RecorderSaveGame->ActorTransforms;
	}

	UE_LOG(LogTemp, Warning, TEXT("Actor Record Loaded"));
}

void ARecorder::PrintCurrentRecord()
{
	for (TSubclassOf<AActor> ActorClass : ActorClassReferences)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reference: %s"), *ActorClass->GetName());
	}

	for (FTransform ActorTransform : ActorTransforms)
	{
		UE_LOG(LogTemp, Warning, TEXT("Transform: %s"), *ActorTransform.ToString());
	}
}

void ARecorder::SpawnActorsFromRecord()
{
	if (ActorClassReferences.Num() == ActorTransforms.Num())
	{
		for (int i = 0; i < ActorTransforms.Num(); i++)
		{
			FActorSpawnParameters SpawnInfo;
			TSubclassOf<AActor> SpawnClass = ActorClassReferences[i];
			FTransform SpawnTransform = ActorTransforms[i];
			GetWorld()->SpawnActor<AActor>(SpawnClass, SpawnTransform, SpawnInfo);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FATAL ERROR"));
	}
}

void ARecorder::SetActiveSaveSlot(int SlotIndex)
{
	ActiveSaveSlot = SlotIndex;
}

