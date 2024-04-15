// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Compressable/Public/CompressableBase.h"
#include "DynamicPushingActor.h"
#include "ForceFieldBase.h"
#include "FallingActorBase.h"
#include "CompressableGameActor.generated.h"


UCLASS()
class UNFRACTURE_API ACompressableGameActor : public ACompressableBase
{
	GENERATED_BODY()

public:
	ACompressableGameActor();

	virtual void Tick(float DeltaTime) override;

	TMap<FString, ADynamicPushingActor*> RegisteredPushingActors;
	TMap<FString, AFallingActorBase*> RegisteredFallingActors;

	void HandleFallingActors();

	void HandleForceFieldActors();

	void HandlePushingActors();

	void SetForcedNodesFromForceFieldActor(
		TArray<int32> targetNodes,
		FVector forceFieldActorLocalLocation,
		float forceFieldStrength,
		int forceFieldRadialDirection,
		int falloffParameter);

	void SetForcedNodesFromFallingActor(TArray<int32> targetNodes, float forceX, float forceY);

	void SetForcedNodesFromPushingActor(
		TArray<int32> targetNodes,
		FVector pushingVector,
		int nodalWidth);

	void CollectForcedNodes(
		TArray<int32> ForcedNodesFromActor,
		TMap<int32, float> ForcedNodesYMagnitudesFromActor,
		TMap<int32, float> ForcedNodesZMagnitudesFromActor);

	virtual void RunFullMeshUpdate();

	virtual void HandleAllForces();

	virtual void HandleVectors();

	void ResetForcedNodes();

	void RegisterPushingActor(FString ActorName, ADynamicPushingActor* PushingActor);

	void UnRegisterPushingActor(FString ActorName);

	void RegisterFallingActor(FString ActorName, AFallingActorBase* FallingActor);

private:
	
	//ForcedNodes (and ForcedNodesFromFallingActor, ForceField, etc...) should ALWAYS have ForcedNodeMagnitudes in Y and Z mapped to them

	TArray<int32> ForcedNodesFromFallingActor;

	TMap<int32, float> ForcedNodeYMagnitudesFromFallingActor;

	TMap<int32, float> ForcedNodeZMagnitudesFromFallingActor;

	TArray<int32> ForcedNodesFromPushingActor;

	TMap<int32, float> ForcedNodeYMagnitudesFromPushingActor;

	TMap<int32, float> ForcedNodeZMagnitudesFromPushingActor;

	TArray<int32> ForcedNodesFromForceFieldActor;

	TMap<int32, float> ForcedNodeYMagnitudesFromForceFieldActor;

	TMap<int32, float> ForcedNodeZMagnitudesFromForceFieldActor;

	UFUNCTION()
	void OnMeshHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);


protected:
	virtual void BeginPlay() override;

};
