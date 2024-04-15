// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <Eigen/Dense>
#include "CompressableBase.h"
#include "UnFractureStructs.h"
#include "UnFractureGameModeBase.generated.h"

using Eigen::MatrixXf;
using Eigen::VectorXf;

UCLASS()
class COMPRESSABLE_API AUnFractureGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AUnFractureGameModeBase();

	UWorld* World;

	AWorldSettings* WorldSettings;

	TArray<FString> MeshNameList;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<AActor*> CompressableBaseList;

	FCompressableData GetCompressableDataByMeshName(FString MeshName);

	TArray<FVector> GetNodePositionsByMeshName(FString MeshName);
	TArray<int32> GetTrianlgeIndicesByMeshName(FString MeshName);
	MatrixXf GetKInverseByMeshName(FString MeshName);
	TArray<int32> GetFixedNodesByMeshName(FString MeshName);
	TArray<int32> GetEdgeNodesByMeshName(FString MeshName);
	TMap<int32, int32> GetNodeMatrixMapByMeshName(FString MeshName);
	TArray<int> GetMeshDensityDataByMeshName(FString MeshName);
	TArray<float> GetMeshSpacingDataByMeshName(FString MeshName);
	MatrixXf GetKByMeshName(FString MeshName);
	float GetElasticModulusByMeshName(FString MeshName);
	

protected:

	virtual void BeginPlay() override;

private:
	void LoadAllMeshData();
	
	TMap<FString, FCompressableData> CompressableDataMap;

	TMap<FString, TArray<FVector>> NodePositionsMap;
	TMap<FString, TArray<int32>> TriangleIndicesMap;
	TMap<FString, MatrixXf> KInverseMap;
	TMap<FString, TArray<int32>> FixedNodesMap;
	TMap<FString, TArray<int32>> EdgeNodesMap;
	TMap<FString, TMap<int32, int32>> NodeMatrixMapMap;
	TMap<FString, TArray<int>> MeshDensityDataMap;
	TMap<FString, TArray<float>> MeshSpacingDataMap;
	TMap<FString, MatrixXf> KMap;
	TMap<FString, float> ElasticModulusMap;

	void GetAllMeshNamesInLevel();

};
