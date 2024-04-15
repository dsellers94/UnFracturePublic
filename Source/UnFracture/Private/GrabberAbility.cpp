// Copyright Daniel Sellers 2023


#include "GrabberAbility.h"
#include "Components/StaticMeshComponent.h"
#include "DynamicPushingActor.h"
#include "DroneAbilityComponent.h"

AGrabberAbility::AGrabberAbility()
{

}

void AGrabberAbility::Fire(bool inputBool)
{
	if (inputBool)
	{
		GetOverlappingActors(OverlappedActors, AActor::StaticClass());

		for (AActor* Actor : OverlappedActors)
		{
			if (Actor->ActorHasTag("FallingActor"))
			{
				AttachedActor = Actor;

				const FAttachmentTransformRules AttachmentTransformRules = FAttachmentTransformRules(EAttachmentRule::KeepWorld, true);

				AttachedActor->AttachToActor(
					DroneOwner,
					AttachmentTransformRules);

				AttachedActor->SetActorEnableCollision(false);

				break;
			}
			else if (Actor->ActorHasTag("PushingActor"))
			{
				ADynamicPushingActor* PushingActor = Cast<ADynamicPushingActor>(Actor);
				if (PushingActor)
				{
					PushingActor->Disengage();
					AbilityComponent->AddAmmo(RecycleClass, 1);
				}
			}
		}
	}
	else
	{
		if (AttachedActor)
		{
			AttachedActor->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			UStaticMeshComponent* AttachedStaticMesh = Cast<UStaticMeshComponent>(AttachedActor->GetRootComponent());
			if (AttachedStaticMesh)
			{
				AttachedActor->SetActorEnableCollision(true);
				AttachedStaticMesh->SetSimulatePhysics(true);
			}
		}
	}
}

void AGrabberAbility::Activate()
{
	Super::Activate();

	SetActorEnableCollision(true);
}

void AGrabberAbility::Deactivate()
{
	Super::Deactivate();

	SetActorEnableCollision(false);
}

