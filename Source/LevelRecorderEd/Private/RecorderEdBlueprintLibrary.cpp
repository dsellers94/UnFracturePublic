// Copyright Daniel Sellers 2023


#include "RecorderEdBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Editor/EditorEngine.h"
#include "Engine/Engine.h"

//This is the important one if I can make it load the existing savegame as it's own class of save game, 
//the others can be commented out if we can't instantiate this uclass (I don't think we can)
void URecorderEdBlueprintLibrary::LoadRecord(
	int SlotIndex, 
	TArray<TSubclassOf<AActor>> &outActorClassReferences, 
	TArray<FTransform> &outActorTransforms,
	TArray<FTagArray> &outActorTagArrays)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting Record Load"));

	URecorderEdSaveGame* RecorderEdSaveGame = Cast<URecorderEdSaveGame>(UGameplayStatics::CreateSaveGameObject(URecorderEdSaveGame::StaticClass()));

	RecorderEdSaveGame = Cast<URecorderEdSaveGame>(UGameplayStatics::LoadGameFromSlot("RecorderEdSaveSlot", SlotIndex));

	if (RecorderEdSaveGame)
	{
		outActorClassReferences = RecorderEdSaveGame->ActorClassReferences;

		outActorTransforms = RecorderEdSaveGame->ActorTransforms;

		outActorTagArrays = RecorderEdSaveGame->ActorTagArrays;

		UE_LOG(LogTemp, Warning, TEXT("Actor Record Loaded"));
	}
}

void URecorderEdBlueprintLibrary::SaveRecord(
	TArray<TSubclassOf<AActor>> ActorClassReferences, 
	TArray<FTransform> ActorTransforms, 
	TArray<FTagArray> ActorTagArrays,
	int SlotIndex)
{
	URecorderEdSaveGame* RecorderEdSaveGame = Cast<URecorderEdSaveGame>(UGameplayStatics::CreateSaveGameObject(URecorderEdSaveGame::StaticClass()));

	RecorderEdSaveGame->ActorClassReferences = ActorClassReferences;

	RecorderEdSaveGame->ActorTransforms = ActorTransforms;

	RecorderEdSaveGame->ActorTagArrays = ActorTagArrays;

	UGameplayStatics::SaveGameToSlot(RecorderEdSaveGame, TEXT("RecorderEdSaveSlot"), SlotIndex);

	UE_LOG(LogTemp, Warning, TEXT("Actor Record Saved"));
}

TArray<FTransform> URecorderEdBlueprintLibrary::RecordTargetActorTransforms(TArray<TSubclassOf<AActor>> TargetParentClasses)
{
	UWorld* World = nullptr;

	if (GEditor->GetPIEWorldContext())
	{
		World = GEditor->GetPIEWorldContext()->World();
	}

	TArray<FTransform> ActorTransforms;

	if (World)
	{
		for (TSubclassOf<AActor> TargetClass : TargetParentClasses)
		{
			TArray<AActor*> TargetClassMembers;
			UGameplayStatics::GetAllActorsOfClass(World, TargetClass, TargetClassMembers);

			for (AActor* TargetActor : TargetClassMembers)
			{
				FTransform TargetActorTransform = TargetActor->GetTransform();

				ActorTransforms.Add(TargetActorTransform);
			}
		}
	}

	return ActorTransforms;
}

TArray<TSubclassOf<AActor>> URecorderEdBlueprintLibrary::RecordTargetActorClasses(TArray<TSubclassOf<AActor>> TargetParentClasses)
{
	UWorld* World = nullptr;

	if (GEditor->GetPIEWorldContext())
	{
		World = GEditor->GetPIEWorldContext()->World();
	}

	TArray<TSubclassOf<AActor>> ActorClassReferences;

	if (World)
	{
		for (TSubclassOf<AActor> TargetClass : TargetParentClasses)
		{
			TArray<AActor*> TargetClassMembers;
			UGameplayStatics::GetAllActorsOfClass(World, TargetClass, TargetClassMembers);

			for (AActor* TargetActor : TargetClassMembers)
			{
				TSubclassOf<AActor> TargetActorClass = TargetActor->GetClass();

				ActorClassReferences.Add(TargetActorClass);
			}
		}
	}

	return ActorClassReferences;
}


TArray<FTagArray> URecorderEdBlueprintLibrary::RecordTargetActorTagArrays(TArray<TSubclassOf<AActor>> TargetParentClasses)
{
	UWorld* World = nullptr;

	if (GEditor->GetPIEWorldContext())
	{
		World = GEditor->GetPIEWorldContext()->World();
	}

	TArray<FTagArray> ActorTagArrays;

	if (World)
	{
		for (TSubclassOf<AActor> TargetClass : TargetParentClasses)
		{
			TArray<AActor*> TargetClassMembers;
			UGameplayStatics::GetAllActorsOfClass(World, TargetClass, TargetClassMembers);

			for (AActor* TargetActor : TargetClassMembers)
			{
				FTagArray ActorTags; 

				ActorTags.TagArray = TargetActor->Tags;

				ActorTagArrays.Add(ActorTags);
			}
		}
	}

	return ActorTagArrays;
}

void URecorderEdBlueprintLibrary::SpawnActorsFromRecord(
	TArray<TSubclassOf<AActor>> ActorClassReferences, 
	TArray<FTransform> ActorTransforms, 
	TArray<FTagArray> ActorTags)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();

	if (ActorClassReferences.Num() == ActorTransforms.Num())
	{
		for (int i = 0; i < ActorTransforms.Num(); i++)
		{
			FActorSpawnParameters SpawnInfo;
			TSubclassOf<AActor> SpawnClass = ActorClassReferences[i];
			FTransform SpawnTransform = ActorTransforms[i];
			FTagArray SpawnTags = ActorTags[i];
			
			AActor* SpawnedActor = World->SpawnActor<AActor>(SpawnClass, SpawnTransform, SpawnInfo);

			for (FName Tag : SpawnTags.TagArray)
			{
				SpawnedActor->Tags.Add(Tag);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("FATAL ERROR"));
	}
}

void URecorderEdBlueprintLibrary::ClearTargetActors(TArray<TSubclassOf<AActor>> inTargetParentClasses)
{
	UWorld* World = GEditor->GetEditorWorldContext().World();

	for (TSubclassOf<AActor> TargetClass : inTargetParentClasses)
	{
		TArray<AActor*> TargetClassMembers;
		UGameplayStatics::GetAllActorsOfClass(World, TargetClass, TargetClassMembers);

		for (AActor* TargetActor : TargetClassMembers)
		{
			World->DestroyActor(TargetActor);
		}
	}
}
