// Copyright Daniel Sellers 2023

#include "PressureField.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

APressureField::APressureField()
{

}

//Still not the ideal solution for a pressure field that I was hoping for because it can directly affect nodes that it does not have line of sight to
//but it will work as a standin in for now. This field gets any nodes on the compressable actor that are closer to the field than the center point is
TArray<int32> APressureField::GetForceFieldTargetNodes(AActor* EffectedCompressable)
{
	ACompressableBase* CompressableActor = Cast<ACompressableBase>(EffectedCompressable);
	TArray<int32> CompressableEdgeNodes = CompressableActor->GetEdgeNodes();
	TArray<FVector> CompressableNodePositions = CompressableActor->GetNodePositions();

	TArray<int32> TargetNodes;

	int CenterPointIndex = CompressableNodePositions.Num() - 1;

	if (CenterPointIndex < 0) return TargetNodes;

	FVector CenterPointLocation = CompressableNodePositions[CenterPointIndex] + EffectedCompressable->GetActorLocation();

	float FieldCenterPointDistance = FVector::Dist(CenterPointLocation, GetActorLocation());

	for (int32 Node : CompressableEdgeNodes)
	{
		FVector NodeLocation = CompressableNodePositions[Node] + EffectedCompressable->GetActorLocation();

		float FieldNodeDistance = FVector::Dist(NodeLocation, GetActorLocation());

		if (FieldNodeDistance < FieldCenterPointDistance)
		{
			TargetNodes.Add(Node);
		}
	}
	//UE_LOG(LogTemp, Display, TEXT("Pressure Field Effected Node Count: %d"), TargetNodes.Num());
	return TargetNodes;
}
