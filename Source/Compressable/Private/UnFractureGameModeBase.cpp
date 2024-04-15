// Copyright Epic Games, Inc. All Rights Reserved.


#include "UnFractureGameModeBase.h"
#include "MeshLoadUtility.h"
#include "Kismet/GameplayStatics.h"


AUnFractureGameModeBase::AUnFractureGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;

	World = GetWorld();
	
	GetAllMeshNamesInLevel();
	LoadAllMeshData();

}

void AUnFractureGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	WorldSettings = World->GetWorldSettings();
}

void AUnFractureGameModeBase::LoadAllMeshData()
{
	for (FString MeshName : MeshNameList)
	{
		if (MeshLoadUtility::CheckValidMeshFile("Files/" + MeshName + "_positions.csv"))
		{
			CompressableDataMap.Add(MeshName, MeshLoadUtility::GetCompressableDataByMeshName(MeshName));
		}
	}
}

void AUnFractureGameModeBase::GetAllMeshNamesInLevel()
{
	UGameplayStatics::GetAllActorsOfClass(World, ACompressableBase::StaticClass(), CompressableBaseList);
	for (AActor* Actor : CompressableBaseList)
	{
		ACompressableBase* CompressableBaseActor = Cast<ACompressableBase>(Actor);
		FString CandidateMeshName = CompressableBaseActor->GetMeshName();
		if (!MeshNameList.Contains(CandidateMeshName))
		{
			MeshNameList.Add(CandidateMeshName);
		}
	}

	for (FString Name : MeshNameList)
	{
		UE_LOG(LogTemp, Warning, TEXT("Name: %s"), *Name);
	}
}

FCompressableData AUnFractureGameModeBase::GetCompressableDataByMeshName(FString MeshName)
{
	return CompressableDataMap[MeshName];
}

TArray<FVector> AUnFractureGameModeBase::GetNodePositionsByMeshName(FString MeshName)
{
	return NodePositionsMap[MeshName];
}

TArray<int32> AUnFractureGameModeBase::GetTrianlgeIndicesByMeshName(FString MeshName)
{
	return TriangleIndicesMap[MeshName];
}

MatrixXf AUnFractureGameModeBase::GetKInverseByMeshName(FString MeshName)
{
	return KInverseMap[MeshName];
}

TArray<int32> AUnFractureGameModeBase::GetFixedNodesByMeshName(FString MeshName)
{
	return FixedNodesMap[MeshName];
}

TArray<int32> AUnFractureGameModeBase::GetEdgeNodesByMeshName(FString MeshName)
{
	return EdgeNodesMap[MeshName];
}

TMap<int32, int32> AUnFractureGameModeBase::GetNodeMatrixMapByMeshName(FString MeshName)
{
	return NodeMatrixMapMap[MeshName];
}

TArray<int> AUnFractureGameModeBase::GetMeshDensityDataByMeshName(FString MeshName)
{
	return MeshDensityDataMap[MeshName];
}

TArray<float> AUnFractureGameModeBase::GetMeshSpacingDataByMeshName(FString MeshName)
{
	return MeshSpacingDataMap[MeshName];
}

MatrixXf AUnFractureGameModeBase::GetKByMeshName(FString MeshName)
{
	return KMap[MeshName];
}

float AUnFractureGameModeBase::GetElasticModulusByMeshName(FString MeshName)
{
	return ElasticModulusMap[MeshName];
}

