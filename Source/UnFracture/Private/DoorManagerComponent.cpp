// Copyright Daniel Sellers 2023


#include "DoorManagerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CompressableGameActor.h"
#include "StressIndicatorComponent.h"
#include "ShipDoor.h"


// Sets default values for this component's properties
UDoorManagerComponent::UDoorManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	DeformationUnlockLimit = 0.30;

	ComponentTimeElapsed = 0;
}


// Called when the game starts
void UDoorManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACompressableGameActor::StaticClass(), Compressables);
	
	CollectDoorKeys();
	BuildDoorKeyMaps();

}


// Called every frame
void UDoorManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ComponentTimeElapsed += DeltaTime;

	if (ComponentTimeElapsed > 5)
	{
		CheckDoorKeys();
	}
}

void UDoorManagerComponent::CollectDoorKeys()
{
	for (AActor* Compressable : Compressables)
	{
		UActorComponent* Component = Compressable->GetComponentByClass(UStressIndicatorComponent::StaticClass());

		TArray<FName> CompressableTags = Compressable->Tags;

		for (FName Tag : CompressableTags)
		{
			if (!KeyNames.Contains(Tag)) KeyNames.Add(Tag);
		}
	}
}

void UDoorManagerComponent::BuildDoorKeyMaps()
{
	for (FName KeyName : KeyNames)
	{
		TArray<AActor*> CompressablesWithTag;
		TArray<AActor*> DoorsWithTag;
		
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACompressableGameActor::StaticClass(), KeyName, CompressablesWithTag);
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AShipDoor::StaticClass(), KeyName, DoorsWithTag);

		if (!TagCompressableMap.Contains(KeyName)) TagCompressableMap.Add(KeyName, CompressablesWithTag);
		if (!TagDoorMap.Contains(KeyName)) TagDoorMap.Add(KeyName, DoorsWithTag);
	}
}

void UDoorManagerComponent::CheckDoorKeys()
{
	for (FName KeyName : KeyNames)
	{
		TArray<AActor*> CompressablesWithTag = TagCompressableMap[KeyName];
		TArray<AActor*> DoorsWithTag = TagDoorMap[KeyName];

		bool DoorUnlocked = true;

		for (AActor* CompressableActor : CompressablesWithTag)
		{
			UActorComponent* Component = CompressableActor->GetComponentByClass(UStressIndicatorComponent::StaticClass());
			UStressIndicatorComponent* Indicator = Cast<UStressIndicatorComponent>(Component);

			float DeformationScale = Indicator->GetDeformationScale();

			if (DeformationScale > DeformationUnlockLimit)
			{
				DoorUnlocked = false;
				break;
			}
		}

		if (DoorUnlocked)
		{
			for (AActor* DoorActor : DoorsWithTag)
			{
				AShipDoor* Door = Cast<AShipDoor>(DoorActor);

				if (Door && !Door->IsOpen)
				{
					Door->ToggleDoor();
				}
			}
		}
		//else
		//{
		//	for (AActor* DoorActor : DoorsWithTag)
		//	{
		//		AShipDoor* Door = Cast<AShipDoor>(DoorActor);

		//		if (Door && Door->IsOpen)
		//		{
		//			Door->ToggleDoor();
		//		}
		//	}
		//}
	}
}

