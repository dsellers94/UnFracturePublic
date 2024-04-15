// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include <Eigen/Dense>
#include "GameFramework/Actor.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshActor.h"
#include "MeshLoadUtility.h"
#include "Kismet/GameplayStatics.h"
#include "UnFractureStructs.h"
#include "CompressableInterface.h"
#include "CompressableBase.generated.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInitializationComplete);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMeshChanged);

UCLASS()
class COMPRESSABLE_API ACompressableBase : public AActor, public ICompressableInterface
{
	GENERATED_BODY()
	
public:	
	// Constructor and Variables
	ACompressableBase();

	UPROPERTY()
	FInitializationComplete OnInitComplete;

	UPROPERTY()
	FMeshChanged OnMeshChanged;

	//Public Variables

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	FString MeshName;

	UPROPERTY()
	FCompressableData CompressableData;

	UPROPERTY(EditAnywhere, BlueprintReadwrite)
	bool MeshIsValid;

	bool StillInterpolating;

	VectorXf ForceVector;

	VectorXf DisplacementVector;

	VectorXf PreviousDisplacementVector;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	URealtimeMeshComponent* RealtimeMeshComponent;

	UPROPERTY(EditAnywhere)
	float Plasticity;

	URealtimeMeshSimple* RealtimeMesh;

	FRealtimeMeshSimpleMeshData MeshData;

	FRealtimeMeshSectionGroupKey GroupKey;

	FRealtimeMeshSectionKey SectionInGroup;

	TArray<int32> ForcedNodes;

	TMap<int32, float> ForcedNodeYMagnitudes;

	TMap<int32, float> ForcedNodeZMagnitudes;

	virtual void Tick(float DeltaTime) override;

	//Interface Utilities

	UFUNCTION(BlueprintCallable)
	virtual TArray<int32> GetEdgeNodes() override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FVector> GetNodePositions() override;

	UFUNCTION(BlueprintCallable)
	virtual TArray<FVector> GetMeshDataPositions() override;

	UFUNCTION()
	virtual FVector GetCompressableLocation() override;

	virtual MatrixXf GetNodeGridMap() override;

	UFUNCTION()
	float GetDeltaY();

	UFUNCTION()
	float GetDeltaZ();

	UFUNCTION()
	FString GetMeshName();

	UFUNCTION()
	TArray<int32> GetTriangleIndices();

	virtual int32 GetClosestSingleNode(FVector ForceLocation) override;

	virtual TArray<int32> GetClosestNodesSorted(FVector TargetPosition, int NodeCount) override;

	int32 GetClosestSingleUndeformedNodeToAppliedForce(FVector ForceLocation);

	TArray<int32> GetClosestNodesToAppliedForce(FVector ForceLocation, int NodalWidth);

	TArray<int32> GetClosestUndeformedNodesToAppliedForce(FVector ForceLocation, int NodalWidth);

	FVector ConvertWorldVectorToLocal(FVector WorldVector);

	// RMC and Eigen Management Functions

	virtual void UpdateMeshPositions();

	virtual void UpdateMeshPositionsDynamic(float Plasticity);

	virtual void UpdateFocalPointPositions();

	virtual void UpdateMesh();

	VectorXf ConstructForceVector(
		TArray<int32> ForcedNodes,
		TArray<int32> FixedNodes,
		TMap<int32, float> forcedNodeYMagnitudes,
		TMap<int32, float> forcedNodeZMagnitudes);

	VectorXf CalculateDisplacementVector();

	bool DisplacementVectorHasChanged();


private:

	//Functions

	void InitializeMesh();

protected:
	virtual void BeginPlay() override;

};

