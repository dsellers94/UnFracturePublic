// Copyright Daniel Sellers 2023

#include "CompressableGameActor.h"


ACompressableGameActor::ACompressableGameActor()
{
	Plasticity = 500;

	OnActorHit.AddDynamic(this, &ACompressableGameActor::OnMeshHit);
}

void ACompressableGameActor::BeginPlay()
{
	Super::BeginPlay();
}

void ACompressableGameActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunFullMeshUpdate();
}

void ACompressableGameActor::RunFullMeshUpdate()
{
	HandleAllForces();

	HandleVectors();

	if (DisplacementVectorHasChanged() || StillInterpolating)
	{
		UpdateMeshPositionsDynamic(Plasticity);
		UpdateFocalPointPositions();
		UpdateMesh();
		OnMeshChanged.Broadcast();
	}

	ResetForcedNodes();
}

void ACompressableGameActor::HandleAllForces()
{
	HandleFallingActors();

	HandleForceFieldActors();

	HandlePushingActors();
}

void ACompressableGameActor::HandleVectors()
{
	ForceVector = ConstructForceVector(ForcedNodes, CompressableData.FixedNodes, ForcedNodeYMagnitudes, ForcedNodeZMagnitudes);

	DisplacementVector = CalculateDisplacementVector();
}

void ACompressableGameActor::ResetForcedNodes()
{
	ForcedNodes.Empty();
	ForcedNodeYMagnitudes.Empty();
	ForcedNodeZMagnitudes.Empty();
}

void ACompressableGameActor::HandleFallingActors()
{
	for (auto pair : RegisteredFallingActors)
	{
		AFallingActorBase* FallingActor = pair.Value;

		FVector FallingActorLocalLocation = ConvertWorldVectorToLocal(FallingActor->GetActorLocation());

		int ClosestNodeIndex = GetClosestSingleNode(FallingActorLocalLocation);
		FVector SmallestSeparationVector = MeshData.Positions[ClosestNodeIndex] - FallingActorLocalLocation;
		
		//int ClosestNodeIndex = GetClosestSingleUndeformedNodeToAppliedForce(FallingActorLocalLocation);
		//FVector SmallestSeparationVector = NodePositions[ClosestNodeIndex] - FallingActorLocalLocation;

		int NodalWidth = 3;

		float Width = FallingActor->GetWidth();
		NodalWidth = FMath::Floor(Width / CompressableData.DeltaY);
		bool CloseEnough = SmallestSeparationVector.Length() < Width;

		if (CloseEnough)
		{
			if (FallingActor->GetActorEnableCollision())
			{
				float FallingActorDownForce = FallingActor->GetDownForce();

				TArray<int32> TargetNodes = GetClosestUndeformedNodesToAppliedForce(FallingActorLocalLocation, NodalWidth);

				SetForcedNodesFromFallingActor(TargetNodes, 0.0, -FallingActorDownForce / NodalWidth);
			}

		}

	}

	CollectForcedNodes(ForcedNodesFromFallingActor, ForcedNodeYMagnitudesFromFallingActor, ForcedNodeZMagnitudesFromFallingActor);

	ForcedNodesFromFallingActor.Empty();
	ForcedNodeYMagnitudesFromFallingActor.Empty();
	ForcedNodeZMagnitudesFromFallingActor.Empty();

}

void ACompressableGameActor::HandleForceFieldActors()
{
	TArray<AActor*> ForceFieldActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AForceFieldBase::StaticClass(), ForceFieldActors);

	for (AActor* Actor : ForceFieldActors)
	{
		AForceFieldBase* ForceFieldActor = Cast<AForceFieldBase>(Actor);
		FVector ForceFieldActorLocalLocation = ConvertWorldVectorToLocal(ForceFieldActor->GetActorLocation());
		float ForceFieldActorEffectiveRange = ForceFieldActor->GetEffectiveRange();

		if (ForceFieldActorLocalLocation.Length() > ForceFieldActorEffectiveRange) continue;

		float ForceFieldStrength = ForceFieldActor->GetForceFieldStrength();
		int ForceFieldRadialDirection = ForceFieldActor->GetForceFieldRadialDirection();
		int FalloffParameter = ForceFieldActor->GetFalloffParameter();

		TArray<int32> TargetNodes = ForceFieldActor->GetForceFieldTargetNodes(this);

		SetForcedNodesFromForceFieldActor(
			TargetNodes,
			ForceFieldActorLocalLocation,
			ForceFieldStrength,
			ForceFieldRadialDirection,
			FalloffParameter);
	}

	CollectForcedNodes(ForcedNodesFromForceFieldActor, ForcedNodeYMagnitudesFromForceFieldActor, ForcedNodeZMagnitudesFromForceFieldActor);

	ForcedNodesFromForceFieldActor.Empty();
	ForcedNodeYMagnitudesFromForceFieldActor.Empty();
	ForcedNodeZMagnitudesFromForceFieldActor.Empty();

}

void ACompressableGameActor::HandlePushingActors()
{
	for (auto& pair : RegisteredPushingActors)
	{
		ADynamicPushingActor* PushingActor = pair.Value;
		if (PushingActor->PushingMode)
		{
			int NodalWidth = FMath::Floor(PushingActor->GetWidth() / CompressableData.DeltaY);
			FVector PushingVector = PushingActor->GetPushingVector();
			FVector PushingActorLocalLocation = ConvertWorldVectorToLocal(PushingActor->GetActorLocation());
			TArray<int32> TargetNodes = PushingActor->GetPushingActorTargetNodes();

			SetForcedNodesFromPushingActor(
				TargetNodes,
				PushingVector,
				NodalWidth);
		}
	}

	CollectForcedNodes(ForcedNodesFromPushingActor, ForcedNodeYMagnitudesFromPushingActor, ForcedNodeZMagnitudesFromPushingActor);

	ForcedNodesFromPushingActor.Empty();
	ForcedNodeYMagnitudesFromPushingActor.Empty();
	ForcedNodeZMagnitudesFromPushingActor.Empty();
}


void ACompressableGameActor::SetForcedNodesFromForceFieldActor(
	TArray<int32> targetNodes,
	FVector forceFieldActorLocalLocation,
	float forceFieldStrength,
	int forceFieldRadialDirection,
	int falloffParameter)
{
	for (int32 TargetNodeIndex : targetNodes)
	{
		FVector NodeForceSeparationVector = CompressableData.NodePositions[TargetNodeIndex] - forceFieldActorLocalLocation;
		FVector NodeForceSeparationVectorNormal = NodeForceSeparationVector.GetSafeNormal();
		float NodeForceDistance = FVector::Dist(CompressableData.NodePositions[TargetNodeIndex], forceFieldActorLocalLocation);
		float ForceY = NodeForceSeparationVectorNormal.Y * forceFieldStrength * forceFieldRadialDirection / FMath::Pow(NodeForceDistance, falloffParameter);
		float ForceZ = NodeForceSeparationVectorNormal.Z * forceFieldStrength * forceFieldRadialDirection / FMath::Pow(NodeForceDistance, falloffParameter);

		if (ForcedNodesFromForceFieldActor.Contains(TargetNodeIndex))
		{
			ForcedNodeYMagnitudesFromForceFieldActor[TargetNodeIndex] += ForceY;
			ForcedNodeZMagnitudesFromForceFieldActor[TargetNodeIndex] += ForceZ;
		}
		else
		{
			ForcedNodesFromForceFieldActor.Add(TargetNodeIndex);
			ForcedNodeYMagnitudesFromForceFieldActor.Add(TargetNodeIndex, ForceY);
			ForcedNodeZMagnitudesFromForceFieldActor.Add(TargetNodeIndex, ForceZ);
		}
	}
}

void ACompressableGameActor::SetForcedNodesFromFallingActor(TArray<int32> targetNodes, float forceY, float forceZ)
{
	for (int32 TargetNodeIndex : targetNodes)
	{
		if (ForcedNodesFromFallingActor.Contains(TargetNodeIndex))
		{
			ForcedNodeYMagnitudesFromFallingActor[TargetNodeIndex] += forceY;
			ForcedNodeZMagnitudesFromFallingActor[TargetNodeIndex] += forceZ;
		}
		else
		{
			ForcedNodesFromFallingActor.Add(TargetNodeIndex);
			ForcedNodeYMagnitudesFromFallingActor.Add(TargetNodeIndex, forceY);
			ForcedNodeZMagnitudesFromFallingActor.Add(TargetNodeIndex, forceZ);
		}
	}
}


//I forgot to convert XY plane to YZ plane here and it was pain the ass to debug, I should really fix that conversion at some point
void ACompressableGameActor::SetForcedNodesFromPushingActor(
	TArray<int32> targetNodes,
	FVector pushingVector,
	int nodalWidth)
{
	for (int32 node : targetNodes)
	{
		float ForceY = pushingVector.Y / nodalWidth;
		float ForceZ = pushingVector.Z / nodalWidth;

		if (ForcedNodesFromPushingActor.Contains(node))
		{
			ForcedNodeYMagnitudesFromPushingActor[node] += ForceY;
			ForcedNodeZMagnitudesFromPushingActor[node] += ForceZ;
		}
		else
		{
			ForcedNodesFromPushingActor.Add(node);
			ForcedNodeYMagnitudesFromPushingActor.Add(node, ForceY);
			ForcedNodeZMagnitudesFromPushingActor.Add(node, ForceZ);
		}
	}
}

void ACompressableGameActor::CollectForcedNodes(
	TArray<int32> ForcedNodesFromActor,
	TMap<int32, float> ForcedNodeYMagnitudesFromActor,
	TMap<int32, float> ForcedNodeZMagnitudesFromActor)
{
	for (int32 Node : ForcedNodesFromActor)
	{
		if (ForcedNodes.Contains(Node))
		{
			ForcedNodeYMagnitudes[Node] += ForcedNodeYMagnitudesFromActor[Node];
			ForcedNodeZMagnitudes[Node] += ForcedNodeZMagnitudesFromActor[Node];
		}
		else
		{
			ForcedNodes.Add(Node);
			ForcedNodeYMagnitudes.Add(Node, ForcedNodeYMagnitudesFromActor[Node]);
			ForcedNodeZMagnitudes.Add(Node, ForcedNodeZMagnitudesFromActor[Node]);
		}
	}
}

void ACompressableGameActor::RegisterPushingActor(FString ActorName, ADynamicPushingActor* PushingActor)
{
	RegisteredPushingActors.Add(ActorName, PushingActor);
}

void ACompressableGameActor::UnRegisterPushingActor(FString ActorName)
{
	RegisteredPushingActors.Remove(ActorName);
}

void ACompressableGameActor::RegisterFallingActor(FString ActorName, AFallingActorBase* FallingActor)
{
	if (!RegisteredFallingActors.Contains(ActorName))
	{
		RegisteredFallingActors.Add(ActorName, FallingActor);
	}
}

void ACompressableGameActor::OnMeshHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	//HandleFallingActor(OtherActor, Hit);
}