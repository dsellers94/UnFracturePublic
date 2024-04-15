// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Eigen/Dense>
#include "GameFramework/Actor.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshActor.h"
#include "RMCTestActor.generated.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

UCLASS()
class UNFRACTURE_API ARMCTestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARMCTestActor();

	UPROPERTY(VisibleAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere)
	URealtimeMeshComponent* RealtimeMeshComponent;

	URealtimeMeshSimple* RealtimeMesh;

	void CreateTriangle();

	void UpdateTriangle(float DeltaTime);

	FRealtimeMeshSimpleMeshData MeshData;

	FRealtimeMeshSectionKey StaticSectionKey;

	FRealtimeMeshSectionGroupKey GroupKey;

	FRealtimeMeshSectionKey SectionInGroup;

	void ReadInNodes(FString NodesFilePath);

	void ReadInTriangles(FString TrianglesFilePath);

	MatrixXf ReadInGlobalMatrixInverse(FString MatrixInverseFilePath);

	void ReadInFixedNodes(FString FixedNodesFilesPath);

	TArray<int32> SetContigousForcedNodes(int stop, int start = 0);

	TArray<FString> GetCSVFile(FString Path);

	//FOR NOW only used for initial MeshData. Update loop should not bother with nodes that are fixed and therefore not reflected in KInverse, ForceVector, etc...
	TArray<FVector> NodePositions;

	TArray<int32> TriangleIndices;

	TArray<int32> FixedNodes;

	TArray<int32> ForcedNodes;

	MatrixXf KInverse;

	VectorXf ForceVector;

	VectorXf DisplacementVector;

	VectorXf ConstructForceVector(TArray<int32> ForcedNodes, TArray<int32> FixedNodes, float ForceX, float ForceY, int ActiveNodeCount);

	VectorXf CalculateDisplacementVector();

	void UpdateMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
