// Copyright Daniel Sellers 2023


#include "GravityField.h"


AGravityField::AGravityField()
{

}

TArray<int32> AGravityField::GetForceFieldTargetNodes(AActor* EffectedCompressable)
{
	ACompressableBase* CompressableActor = Cast<ACompressableBase>(EffectedCompressable);
	TArray<int32> CompressableEdgeNodes = CompressableActor->GetEdgeNodes();

	return CompressableEdgeNodes;
}

