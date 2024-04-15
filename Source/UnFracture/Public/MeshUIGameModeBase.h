// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeshGenerator.h"
#include "NodePositionMarker.h"
#include "Json.h"
#include "MeshUIGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AMeshUIGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	AMeshUIGameModeBase();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AMeshGenerator> MeshGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AMeshGenerator* SpawnedMeshGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ANodePositionMarker> CornerNodeMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ANodePositionMarker> OriginMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ANodePositionMarker> EdgeNodeMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ANodePositionMarker> InternalNodeMarkerClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString MeshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString GenerationDataFileName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TArray<FVector> CornerNodePositions;

	TArray<int> MeshDensity;

	TArray<FVector> FixedNodeBoxes;

	TArray<FVector> FocalPointPositions;

	float ElasticMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Ny;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Nz;

	UPROPERTY()
	float DeltaY;

	UPROPERTY()
	float DeltaZ;

	UPROPERTY()
	float YMin;

	UPROPERTY()
	float ZMin;

	UPROPERTY(EditAnywhere)
	int OrthogDelta;

	UFUNCTION(BlueprintCallable)
	void AddToCornerNodePositions(FVector Position);

	UFUNCTION(BlueprintCallable)
	void ConfirmCornerPositions();

	UFUNCTION()
	TArray<float> CalculateMeshSpacingData(TArray<FVector> inputCornerNodePositions, int inputNy, int inputNz);

	UFUNCTION()
	TArray<FVector> AutoCorrectEdgeLines(TArray<FVector> inputCornerNodePositions, float inputDeltaY, float inputDeltaZ);

	UFUNCTION(BlueprintCallable)
	void IngestMeshDensityInput(int inputNy, int inputNz);

	UFUNCTION(BlueprintCallable)
	void IngestAndPassMeshName(FString Name);

	UFUNCTION(BlueprintCallable)
	void IngestAndPassElasticMod(float elasticMod);

	UFUNCTION(BlueprintCallable)
	void IngestAndPassFixedNodes(FVector Start, FVector End);

	UFUNCTION(BlueprintCallable)
	void IngestAndPassFocalPoint(FVector FocalPointPosition);

	UFUNCTION(BlueprintCallable)
	void ClearAllNodePositionMarkersOfSelectedClass(TSubclassOf<ANodePositionMarker> NodeMarkerClassToDestroy);

	UFUNCTION()
	void PlaceCorrectedCornerNodePositionsMarkers();

	UFUNCTION()
	void PlaceEdgeNodePositionMarkers();

	UFUNCTION(BlueprintCallable)
	void PlaceInternalNodePositionMarkers();

	UFUNCTION(BlueprintCallable)
	void FinishMeshGeneration();

	UFUNCTION(BlueprintCallable)
	void SaveMesh();

	UFUNCTION(BlueprintCallable)
	void ResetAll();
};
