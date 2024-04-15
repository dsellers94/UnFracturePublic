// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Eigen/Dense>
#include "MeshLoadUtility.h"
#include "Kismet/KismetMathLibrary.h"
#include "Compressable/Public/UnFractureStructs.h"
#include "MeshGenerator.generated.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

UCLASS()
class UNFRACTURE_API AMeshGenerator : public AActor
{
	GENERATED_BODY()
	
public:	

	AMeshGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Ny;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Nz;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ElasticMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString MeshName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString GenerationDataFile;

	float DeltaY;
	
	float DeltaZ;

	UPROPERTY(EditAnywhere)
	float Area;

	TArray<FFEANode> CornerNodes;
	
	TArray<FEdgeLine> EdgeLines;
	
	TArray<FFEANode> Nodes;
	
	TArray<FFEANode> EdgeNodes;
	
	MatrixXf NodeGridMap;
	
	TArray<int32> EdgeNodeIndices;
	
	TArray<int32> FixedNodeIndices;
	
	TArray<FFEATriangle> Triangles;
	
	TArray<FVector> Positions;
	
	MatrixXf KReduxInverse;
	
	MatrixXf KReduxInversePrime;

	MatrixXf KRedux;

	MatrixXf KReduxPrime;
	
	TMap<int32, int32> EdgeNodeKMatrixMap;

	TArray<FFocalPoint> FocalPoints;
	
	void CreateCornerNodesFromPositions(TArray<FVector> cornerNodePositions);

	void DefineEdgesAndGrid(TArray<FFEANode> cornerNodes, int ny, int nz);

	virtual void GenerateMesh(
		TArray<FEdgeLine> edgeLines, 
		int ny, 
		int nz, 
		float deltaY,
		float deltaZ);

	void GenerateMeshConnections(MatrixXf nodeGridMap, int Ny, int Nz);

	void GenerateSeparationVectors();

	void AddSawToothDiagonalsToEdgeNodes();

	void ExtractEdgeNodeIndices();

	void SetFixedNodesDefault();

	void GenerateCentralTrianglesByRotation();

	void ExtractPositionsWithCenter();

	MatrixXf GenerateGlobalMatrix(float E, float A);

	MatrixXf ReduceMatrix(MatrixXf K);

	MatrixXf GenerateKReduxInverse(MatrixXf KRedux);

	MatrixXf ReduceMatrixPrime(MatrixXf kReduxInverse);

private:

	void WritePositions(FString Path);

	void WriteTriangles(FString Path);

	void WriteGlobalMatrixInverse(FString Path);

	void WriteFixedNodes(FString Path);

	void WriteEdgeNodes(FString Path);

	void WriteNodeMatrixMap(FString Path);

	void WriteNodeGridMap(FString Path);

	void WriteMetaData(FString Path);

	void WriteGlobalMatrix(FString Path);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FVector> CornerNodePositions;

	UPROPERTY()
	FVector CenterPointPosition;

	UFUNCTION(BlueprintCallable)
	void GenerateAllFromCornerPositions(TArray<FVector> inputCornerNodePositions);

	UFUNCTION(BlueprintCallable)
	virtual void GeneratePhaseOne(TArray<FVector> inputCornerNodePositions);

	UFUNCTION(BlueprintCallable)
	void SetFixedNodes(TArray<int32> fixedNodes);

	UFUNCTION(BlueprintCallable)
	TArray<int32> SetFixedNodesInBox(FVector Start, FVector End);

	UFUNCTION(BlueprintCallable)
	void AddFocalPoint(FVector FocalPointPosition);

	UFUNCTION(BLueprintCallable)
	virtual void GeneratePhaseTwo();

	UFUNCTION(BlueprintCallable)
	void WriteAllToCSV();

	UFUNCTION()
	void SetNy(int inputNy);

	UFUNCTION()
	void SetNz(int inputNz);

	UFUNCTION()
	void SetMeshName(FString Name);

	UFUNCTION()
	void SetElasticMod(float inputElasticMod);

	UFUNCTION()
	TArray<int> GetMeshDensityData();

	UFUNCTION()
	TArray<float> GetMeshSpacingData();

	UFUNCTION()
	TArray<FVector> GetNodePositions();

	UFUNCTION()
	TArray<int32> GetEdgeNodeIndices();

	virtual void ResetMeshGenerator();
};





//void AMeshGenerator::GenerateCentralTriangles()
//{
//	int CenterNodeIndex = Nodes[Nodes.Num() - 1].Index + 1;
//	FFEANode TriangulationNode = Nodes[EdgeNodeIndices[0]];
//	FFEANode NextTriangulationNode = Nodes[EdgeNodeIndices[EdgeNodeIndices.Num() - 1]];
//	bool UpperRightMode = false;
//	bool DownMode = false;
//	int LimitCount = 0;
//
//	while (NextTriangulationNode != Nodes[EdgeNodeIndices[0]])
//	{
//		int LowerRightNeighborIndex = -1;
//		int RightNeighborIndex = -1;
//		int UpperRightNeighborIndex = -1;
//		int UpperNeighborIndex = -1;
//		int UpperLeftNeighborIndex = -1;
//		int LeftNeighborIndex = -1;
//		int LowerLeftNeighborIndex = -1;
//		int LowerNeighborIndex = -1;
//		int LocalXIndex = TriangulationNode.XIndex;
//		int LocalYIndex = TriangulationNode.YIndex;
//
//		//A Better way to do this might be: use the node grid map to sort neighbors, only one check per coordinate set: is it a neighbor?
//		//Procedure will be: check each coordiante set: does a neighbor live there? It is an edge node? Are we in the correct travel mode? if all those are true
//		// create the triangle and break. Much simpler, will merge two loops into one
//		// Tried this, didn't work as implemented because it tries to access points that don't exist on the NodeGridMap. Could try again with checks for edges
//		// but I don't feel like it right now. I also just combine them into one loop... 
//
//		for (int j = 0; j < TriangulationNode.NeighborIndices.Num(); j++)
//		{
//			FFEANode CandidateNode = Nodes[TriangulationNode.NeighborIndices[j]];
//			int32 CandidateNodeIndex = TriangulationNode.NeighborIndices[j];
//			if (CandidateNode.XIndex == LocalXIndex + 1)
//			{
//				if (CandidateNode.YIndex == LocalYIndex - 1)
//				{
//					LowerRightNeighborIndex = CandidateNodeIndex;
//				}
//				else if (CandidateNode.YIndex == LocalYIndex)
//				{
//					RightNeighborIndex = CandidateNodeIndex;
//				}
//				else if (CandidateNode.YIndex == LocalYIndex + 1)
//				{
//					UpperRightNeighborIndex = CandidateNodeIndex;
//				}
//			}
//			else if (CandidateNode.YIndex == LocalYIndex + 1)
//			{
//				if (CandidateNode.XIndex == LocalXIndex)
//				{
//					UpperNeighborIndex = CandidateNodeIndex;
//				}
//				else if (CandidateNode.XIndex == LocalXIndex - 1)
//				{
//					UpperLeftNeighborIndex = CandidateNodeIndex;
//				}
//			}
//			else if (CandidateNode.XIndex == LocalXIndex - 1)
//			{
//				if (CandidateNode.YIndex == LocalYIndex)
//				{
//					LeftNeighborIndex = CandidateNodeIndex;
//				}
//				else if (CandidateNode.YIndex == LocalYIndex - 1)
//				{
//					LowerLeftNeighborIndex = CandidateNodeIndex;
//				}
//			}
//			else if (CandidateNode.YIndex == LocalYIndex - 1 && CandidateNode.XIndex == LocalXIndex)
//			{
//				LowerNeighborIndex = CandidateNodeIndex;
//			}
//		}
//
//		//Build triangles and choose the next triangulation node by checking lower left neighbor first and moving CCW, Unless we've started traveling up or back down
//		if (LowerRightNeighborIndex != -1 && EdgeNodeIndices.Contains(LowerRightNeighborIndex) && !UpperRightMode)
//		{
//			NextTriangulationNode = Nodes[LowerRightNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//		}
//		else if (RightNeighborIndex != -1 && EdgeNodeIndices.Contains(RightNeighborIndex) && !UpperRightMode)
//		{
//			NextTriangulationNode = Nodes[RightNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//		}
//		else if (UpperRightNeighborIndex != -1 && EdgeNodeIndices.Contains(UpperRightNeighborIndex) && !UpperRightMode)
//		{
//			NextTriangulationNode = Nodes[UpperRightNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//		}
//		else if (UpperNeighborIndex != -1 && EdgeNodeIndices.Contains(UpperNeighborIndex) && !DownMode)
//		{
//			NextTriangulationNode = Nodes[UpperNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//			UpperRightMode = true;
//		}
//		else if (UpperLeftNeighborIndex != -1 && EdgeNodeIndices.Contains(UpperLeftNeighborIndex) && !DownMode)
//		{
//			NextTriangulationNode = Nodes[UpperLeftNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//			UpperRightMode = true;
//		}
//		else if (LeftNeighborIndex != -1 && EdgeNodeIndices.Contains(LeftNeighborIndex) && !DownMode)
//		{
//			NextTriangulationNode = Nodes[LeftNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//			UpperRightMode = true;
//		}
//		else if (LowerLeftNeighborIndex != -1 && EdgeNodeIndices.Contains(LowerLeftNeighborIndex) && !DownMode)
//		{
//			NextTriangulationNode = Nodes[LowerLeftNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//		}
//		else if (LowerNeighborIndex != -1 && EdgeNodeIndices.Contains(LowerNeighborIndex))
//		{
//			NextTriangulationNode = Nodes[LowerNeighborIndex];
//			FFEATriangle NewTriangle = FFEATriangle(TriangulationNode.Index, NextTriangulationNode.Index, CenterNodeIndex);
//			Triangles.Add(NewTriangle);
//			DownMode = true;
//		}
//
//		LimitCount += 1;
//		if (LimitCount > EdgeNodes.Num() + 5) break;
//		TriangulationNode = NextTriangulationNode;
//	}
//	UE_LOG(LogTemp, Display, TEXT("Triangle Limit Count: %d"), LimitCount);
//	UE_LOG(LogTemp, Display, TEXT("Triangles: %d"), Triangles.Num());
//	UE_LOG(LogTemp, Display, TEXT("Edge Nodes In Triangulation: %d"), EdgeNodeIndices.Num());
//}

	//int PreviousValue = NodeGridMap[0][0];
	//for (int i = 0; i < NodeGridMap.Num(); i++)
	//{
	//	for (int j = 0; j < NodeGridMap[i].Num(); j++)
	//	{
	//		int CurrentValue = NodeGridMap[i][j];
	//		if (CurrentValue != -1 && PreviousValue == -1)
	//		{
	//			if (!EdgeNodeIndices.Contains(Nodes[CurrentValue].Index))
	//			{
	//				EdgeNodes.Add(Nodes[CurrentValue]);
	//				EdgeNodeIndices.Add(Nodes[CurrentValue].Index);
	//			}
	//		}
	//		else if (CurrentValue == -1 && PreviousValue != -1)
	//		{
	//			if (!EdgeNodeIndices.Contains(Nodes[PreviousValue].Index))
	//			{
	//				EdgeNodes.Add(Nodes[PreviousValue]);
	//				EdgeNodeIndices.Add(Nodes[PreviousValue].Index);
	//			}
	//		}
	//		if (CurrentValue != -1 && (j == NodeGridMap[i].Num() - 1 || j == 0))
	//		{
	//			if (!EdgeNodeIndices.Contains(Nodes[CurrentValue].Index))
	//			{
	//				EdgeNodes.Add(Nodes[CurrentValue]);
	//				EdgeNodeIndices.Add(Nodes[CurrentValue].Index);
	//			}
	//		}
	//		if (CurrentValue != -1 && (i == NodeGridMap.Num() - 1 || i == 0))
	//		{
	//			if (!EdgeNodeIndices.Contains(Nodes[CurrentValue].Index))
	//			{
	//				EdgeNodes.Add(Nodes[CurrentValue]);
	//				EdgeNodeIndices.Add(Nodes[CurrentValue].Index);
	//			}
	//		}
	//		PreviousValue = CurrentValue;
	//	}
	//}