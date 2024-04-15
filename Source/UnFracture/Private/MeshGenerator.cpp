// Copyright Daniel Sellers 2023

#include "MeshGenerator.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

// Sets default values
AMeshGenerator::AMeshGenerator()
{
	PrimaryActorTick.bCanEverTick = true;

	Ny = 40;
	Nz = 40;

	Area = 1.0;
	ElasticMod = 50.0;

	MeshName = "CppTestMesh3";
}

void AMeshGenerator::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("I Exist Now. I Am The Mesh Man"));
}

void AMeshGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeshGenerator::GenerateAllFromCornerPositions(TArray<FVector> inputCornerNodePositions)
{
	CreateCornerNodesFromPositions(inputCornerNodePositions);
	DefineEdgesAndGrid(CornerNodes, Ny, Nz);
	GenerateMesh(EdgeLines, Ny, Nz, DeltaY, DeltaZ);
	GenerateMeshConnections(NodeGridMap, Ny, Nz);
	GenerateSeparationVectors();
	ExtractEdgeNodeIndices();
	AddSawToothDiagonalsToEdgeNodes();
	SetFixedNodesDefault();
	ExtractPositionsWithCenter();
	GenerateCentralTrianglesByRotation(); //CHANGED
	MatrixXf KGlobal = GenerateGlobalMatrix(ElasticMod, Area);
	KRedux = ReduceMatrix(KGlobal);
	KReduxInverse = GenerateKReduxInverse(KRedux);
	KReduxInversePrime = ReduceMatrixPrime(KReduxInverse);
	KReduxPrime = ReduceMatrixPrime(KRedux);
}

void AMeshGenerator::GeneratePhaseOne(TArray<FVector> inputCornerNodePositions)
{
	CreateCornerNodesFromPositions(inputCornerNodePositions);
	DefineEdgesAndGrid(CornerNodes, Ny, Nz);
	GenerateMesh(EdgeLines, Ny, Nz, DeltaY, DeltaZ);
	GenerateMeshConnections(NodeGridMap, Ny, Nz);
	GenerateSeparationVectors();
	ExtractEdgeNodeIndices();
	AddSawToothDiagonalsToEdgeNodes();
	UE_LOG(LogTemp, Display, TEXT("Generated Mesh From Corners, Waiting for Fixed Nodes"));
	
	//for (FEdgeLine EdgeLine : EdgeLines)
	//{
	//	EdgeLine.LogEdgeLineResults();
	//}
}

void AMeshGenerator::SetFixedNodes(TArray<int32> fixedNodes)
{
	for (int NodeIndex : fixedNodes)
	{
		FixedNodeIndices.Add(NodeIndex);
	}
}

TArray<int32> AMeshGenerator::SetFixedNodesInBox(FVector Start, FVector End)
{
	TArray<int32> FixedNodeList;
	float LowerBound = FMath::Min(Start.Z, End.Z);
	float UpperBound = FMath::Max(Start.Z, End.Z);
	float LeftBound = FMath::Min(Start.Y, End.Y);
	float RightBound = FMath::Max(Start.Y, End.Y);
	for (int32 NodeIndex : EdgeNodeIndices)
	{
		FVector CandidateNodePosition = Nodes[NodeIndex].Position;
		if (CandidateNodePosition.Z > LowerBound && CandidateNodePosition.Z < UpperBound && CandidateNodePosition.Y > LeftBound && CandidateNodePosition.Y < RightBound)
		{
			FixedNodeList.Add(NodeIndex);
			FixedNodeIndices.Add(NodeIndex);
		}
	}
	return FixedNodeList;
}

void AMeshGenerator::AddFocalPoint(FVector FocalPointPosition)
{
	FFocalPoint NewFocalPoint = FFocalPoint();
	NewFocalPoint.Position = FocalPointPosition;
	FocalPoints.Add(NewFocalPoint);
}

void AMeshGenerator::GeneratePhaseTwo()
{
	ExtractPositionsWithCenter();
	GenerateCentralTrianglesByRotation(); 
	MatrixXf KGlobal = GenerateGlobalMatrix(ElasticMod, Area);
	KRedux = ReduceMatrix(KGlobal);
	KReduxInverse = GenerateKReduxInverse(KRedux);
	KReduxInversePrime = ReduceMatrixPrime(KReduxInverse);
	KReduxPrime = ReduceMatrixPrime(KRedux);
}

void AMeshGenerator::WriteAllToCSV()
{
	WritePositions("Files/" + MeshName + "_positions.csv");
	WriteTriangles("Files/" + MeshName + "_triangles.csv");
	WriteGlobalMatrixInverse("Files/" + MeshName + "_KReduxInversePrime.csv");
	WriteFixedNodes("Files/" + MeshName + "_FixedNodes.csv");
	WriteEdgeNodes("Files/" + MeshName + "_EdgeNodes.csv");
	WriteNodeMatrixMap("Files/" + MeshName + "_EdgeNodeKMatrixMap.csv");
	WriteMetaData("Files/" + MeshName + "_MetaData.csv");
	WriteGlobalMatrixInverse("Files/" + MeshName + "_KReduxPrime.csv");
	WriteNodeGridMap("Files/" + MeshName + "_NodeGridMap.csv");
}


void AMeshGenerator::CreateCornerNodesFromPositions(TArray<FVector> cornerNodePositions)
{
	for (FVector CornerNodePosition : cornerNodePositions)
	{
		FFEANode NewCornerNode = FFEANode();
		NewCornerNode.IsCornerNode = true;
		NewCornerNode.Position = CornerNodePosition;
		CornerNodes.Add(NewCornerNode);
	}
}

void AMeshGenerator::DefineEdgesAndGrid(TArray<FFEANode> cornerNodes, int ny, int nz)
{
	float YMin = 1000000.0;
	float YMax = - 1000000.0;
	float ZMin = 1000000.0;
	float ZMax = - 1000000.0;

	//Manually create first line between last cornernode and first cornernode
	FEdgeLine NewEdgeLine = FEdgeLine();
	NewEdgeLine.StartPoint = cornerNodes[cornerNodes.Num() - 1].Position;
	NewEdgeLine.EndPoint = cornerNodes[0].Position;
	NewEdgeLine.Calculate();
	EdgeLines.Add(NewEdgeLine);
	float YPos = cornerNodes[cornerNodes.Num() - 1].Position.Y;
	float ZPos = cornerNodes[cornerNodes.Num() - 1].Position.Z;
	if (YPos > YMax) YMax = YPos;
	if (YPos < YMin) YMin = YPos;
	if (ZPos > ZMax) ZMax = ZPos;
	if (ZPos < ZMin) ZMin = ZPos;
	
	//Creeate the rest of the line from the first back around to the last with this loop
	for (int i = 0; i < cornerNodes.Num() - 1; i++)
	{
		NewEdgeLine = FEdgeLine();
		NewEdgeLine.StartPoint = cornerNodes[i].Position;
		NewEdgeLine.EndPoint = cornerNodes[i + 1].Position;
		NewEdgeLine.Calculate();
		EdgeLines.Add(NewEdgeLine);
		YPos = cornerNodes[i].Position.Y;
		ZPos = cornerNodes[i].Position.Z;
		if (YPos > YMax) YMax = YPos;
		if (YPos < YMin) YMin = YPos;
		if (ZPos > ZMax) ZMax = ZPos;
		if (ZPos < ZMin) ZMin = ZPos;
	}

	DeltaY = (YMax - YMin) / ny;
	DeltaZ = (ZMax - ZMin) / nz;
}

void AMeshGenerator::GenerateMesh( // Change for ConcaveMeshGenerator
	TArray<FEdgeLine> edgeLines,
	int ny,
	int nz,
	float deltaY,
	float deltaZ)
{
	int IndexCount = 0;

	NodeGridMap = MatrixXf(ny + 1, nz + 1);

	for (int i = 0; i <= Ny; i++)
	{
		for (int j = 0; j <= Nz; j++)
		{
			bool ValidCandidate = true;

			FFEANode NewNode = FFEANode();
			NewNode.Index = IndexCount;
			NewNode.YIndex = i;
			NewNode.ZIndex = j;

			NewNode.Position = FVector(0.0, i * deltaY, j * deltaZ);

			for (int k = 0; k < edgeLines.Num(); k++)
			{
				if (!edgeLines[k].IsVertical && !edgeLines[k].IsHorizontal)
				{
					float ZBoundary = edgeLines[k].m * (i * deltaY) + edgeLines[k].b;

					if (edgeLines[k].IsLowerBoundary)
					{
						if (j * deltaZ < ZBoundary)
						{
							ValidCandidate = false;
							break;
						}
					}
					else if (edgeLines[k].IsUpperBoundary)
					{
						if (j * deltaZ > ZBoundary)
						{
							ValidCandidate = false;
							break;
						}
					}
				}
				else if (edgeLines[k].IsVertical)
				{
					if (i * deltaY == edgeLines[k].StartPoint.Y)
					{
						NewNode.IsEdgeNode = true;
					}
				}
				else if (edgeLines[k].IsHorizontal)
				{
					if (j * deltaZ == edgeLines[k].StartPoint.Z)
					{
						NewNode.IsEdgeNode = true;
					}
				}
			}

			if (ValidCandidate)
			{
				Nodes.Add(NewNode);
				NodeGridMap(i, j) = NewNode.Index;
				IndexCount += 1;

				if (NewNode.IsEdgeNode) EdgeNodes.Add(NewNode);
			}
			else
			{
				NodeGridMap(i, j) = -1;
			}
		}
	
	}
}

void AMeshGenerator::GenerateMeshConnections(MatrixXf nodeGridMap, int ny, int nz)
{
	for (int i = 0; i < Nodes.Num(); i++)
	{
		FFEANode ConnectingNode = Nodes[i];

		int32 RightNeighborIndex = -1;
		int32 UpperRightNeighborIndex = -1;
		int32 UpperNeighborIndex = -1;
		int32 UpperLeftNeighborIndex = -1;
		int32 LowerNeighborIndex = -1;
		int32 LeftNeighborIndex = -1;

		int32 ConnectingNodeYIndex = ConnectingNode.YIndex;
		int32 ConnectingNodeZIndex = ConnectingNode.ZIndex;

		//Find the index of neighbors above and left and right, lower neighbors are taken care of by previous nodes
		if (ConnectingNodeYIndex != ny)
		{
			RightNeighborIndex = nodeGridMap(ConnectingNodeYIndex + 1, ConnectingNodeZIndex);
			
			if (ConnectingNodeZIndex != nz)
			{
				UpperRightNeighborIndex = nodeGridMap(ConnectingNodeYIndex + 1, ConnectingNodeZIndex + 1);
			}
		}
		if (ConnectingNodeZIndex != nz)
		{
			UpperNeighborIndex = nodeGridMap(ConnectingNodeYIndex, ConnectingNodeZIndex + 1);

			if (ConnectingNodeYIndex != 0)
			{
				UpperLeftNeighborIndex = nodeGridMap(ConnectingNodeYIndex - 1, ConnectingNodeZIndex + 1);
			}
		}
		if (ConnectingNodeZIndex != 0)
		{
			LowerNeighborIndex = nodeGridMap(ConnectingNodeYIndex, ConnectingNodeZIndex - 1);
		}
		if (ConnectingNodeYIndex != 0)
		{
			LeftNeighborIndex = nodeGridMap(ConnectingNodeYIndex - 1, ConnectingNodeZIndex);
		}

		//Assign neighbor relationships with Right, Upper, UpperRight and UpperLeft Neighbors, if they exist
		if (RightNeighborIndex != -1)
		{
			Nodes[i].NeighborIndices.Add(RightNeighborIndex);
			Nodes[RightNeighborIndex].NeighborIndices.Add(i);
		}
		if (UpperNeighborIndex != -1)
		{
			Nodes[i].NeighborIndices.Add(UpperNeighborIndex);
			Nodes[UpperNeighborIndex].NeighborIndices.Add(i);
		}
		if (UpperRightNeighborIndex != -1)
		{
			Nodes[i].NeighborIndices.Add(UpperRightNeighborIndex);
			Nodes[UpperRightNeighborIndex].NeighborIndices.Add(i);
		}
		if (UpperLeftNeighborIndex != -1)
		{
			Nodes[i].NeighborIndices.Add(UpperLeftNeighborIndex);
			Nodes[UpperLeftNeighborIndex].NeighborIndices.Add(i);
		}
	}
}

void AMeshGenerator::GenerateSeparationVectors()
{
	for (int i = 0; i < Nodes.Num(); i++)
	{
		for (int32 Neighbor : Nodes[i].NeighborIndices)
		{
			FVector LocalPosition = Nodes[i].Position;
			FVector NeighborPosition = Nodes[Neighbor].Position;

			FVector SeparationVector = NeighborPosition - LocalPosition;

			Nodes[i].NeighborSeparationVectors.Add(SeparationVector);
		}
	}
}

void AMeshGenerator::ExtractEdgeNodeIndices()
{
	for (FFEANode Node : EdgeNodes)
	{
		EdgeNodeIndices.Add(Node.Index);
	}
}

void AMeshGenerator::AddSawToothDiagonalsToEdgeNodes()
{
	for (int i = 0; i < NodeGridMap.rows(); i++)
	{
		for (int j = 0; j < NodeGridMap.cols(); j++)
		{
			int CurrentValue = NodeGridMap(i, j);

			if (CurrentValue != -1)
			{
				if (i == 0 || i == NodeGridMap.rows() - 1 || j == 0 || j == NodeGridMap.cols() - 1)
				{
					if (!EdgeNodeIndices.Contains(Nodes[CurrentValue].Index))
					{
						Nodes[CurrentValue].IsEdgeNode = true;
						EdgeNodes.Add(Nodes[CurrentValue]);
						EdgeNodeIndices.Add(Nodes[CurrentValue].Index);
					}
				}
				else if (NodeGridMap(i, j + 1) == -1 || NodeGridMap(i, j - 1) == -1 || NodeGridMap(i + 1, j) == -1 || NodeGridMap(i - 1, j) == -1)
				{
					if (!EdgeNodeIndices.Contains(Nodes[CurrentValue].Index))
					{
						Nodes[CurrentValue].IsEdgeNode = true;
						EdgeNodes.Add(Nodes[CurrentValue]);
						EdgeNodeIndices.Add(Nodes[CurrentValue].Index);
					}
				}
			}
		}
	}
}

void AMeshGenerator::SetFixedNodesDefault()
{
	for (int i = 0; i < Nz; i++)
	{
		if (EdgeNodeIndices.Contains(i))
		{
			FixedNodeIndices.Add(i);
		}
	}
}

void AMeshGenerator::ExtractPositionsWithCenter() 
{
	for (int i = 0; i < Nodes.Num(); i++)
	{
		Positions.Add(Nodes[i].Position);
	}
	CenterPointPosition = FVector(0.0, (Ny / 2) * DeltaY, (Nz / 2) * DeltaZ);
	Positions.Add(CenterPointPosition);
}

void AMeshGenerator::GenerateCentralTrianglesByRotation()
{
	int CenterNodeIndex = Nodes[Nodes.Num() - 1].Index + 1;
	TMap<int32, float> IndexRotationMap;

	FVector ReferenceAxisNormal = FVector(0.0, 1.0, 0.0);
	for (int EdgeNodeIndex : EdgeNodeIndices)
	{
		FVector SeparationVector = Nodes[EdgeNodeIndex].Position - CenterPointPosition;
		float VectorDot = (SeparationVector.Y * ReferenceAxisNormal.Y + SeparationVector.Z * ReferenceAxisNormal.Z)/SeparationVector.Length();
		float Theta = UKismetMathLibrary::Acos(VectorDot);
		if (SeparationVector.Z < 0)
		{
			IndexRotationMap.Add(EdgeNodeIndex, 2 * PI - Theta);
		}
		else
		{
			IndexRotationMap.Add(EdgeNodeIndex, Theta);
		}
	}

	IndexRotationMap.ValueSort([](float A, float B) {
		return A < B;
		});

	TArray<int32> EdgeNodesSortedByRotation;

	for (auto& pair : IndexRotationMap)
	{
		EdgeNodesSortedByRotation.Add(pair.Key);
	}

	for (int i = 0; i < EdgeNodesSortedByRotation.Num() - 1; i++)
	{
		FFEATriangle NewTriangle = FFEATriangle(EdgeNodesSortedByRotation[i], EdgeNodesSortedByRotation[i + 1], CenterNodeIndex);
		Triangles.Add(NewTriangle);
	}
	FFEATriangle NewTriangle = FFEATriangle(EdgeNodesSortedByRotation[EdgeNodesSortedByRotation.Num() - 1], EdgeNodesSortedByRotation[0], CenterNodeIndex);
	Triangles.Add(NewTriangle);
}


MatrixXf AMeshGenerator::GenerateGlobalMatrix(float E, float A)
{
	MatrixXf K(2 * Nodes.Num(), 2 * Nodes.Num());

	for (int i = 0; i < Nodes.Num(); i++)
	{
		for (int j = 0; j < Nodes[i].NeighborIndices.Num(); j++)
		{
			FVector SeparationVector = Nodes[i].NeighborSeparationVectors[j];
			float SeparationMagnitude = SeparationVector.Length();

			// Y Component of Kii
			K(2 * i, 2 * i) += E * A * FMath::Abs(SeparationVector.Y / FMath::Pow(SeparationMagnitude, 2));
			// Z Component of Kii
			K(2 * i + 1, 2 * i + 1) += E * A * FMath::Abs(SeparationVector.Z / FMath::Pow(SeparationMagnitude, 2));
			// Y Component of Kij
			K(2 * i, 2 * Nodes[i].NeighborIndices[j]) += - E * A * FMath::Abs(SeparationVector.Y / FMath::Pow(SeparationMagnitude, 2));
			// Z Component of Kij
			K(2 * i + 1, 2 * Nodes[i].NeighborIndices[j] + 1) += - E * A * FMath::Abs(SeparationVector.Z / FMath::Pow(SeparationMagnitude, 2));

		}
	}
	return K;
}

MatrixXf AMeshGenerator::ReduceMatrix(MatrixXf K)
{
	MatrixXf kRedux(K.rows() - 2 * FixedNodeIndices.Num(), K.rows() - 2 * FixedNodeIndices.Num());
	
	TArray<TArray<float>> KReduxArray;
	
	for (int i = 0; i < K.rows() / 2; i++)
	{
		TArray<float> KReduxSubZero;
		TArray<float> KReduxSubOne;
		if (!FixedNodeIndices.Contains(i))
		{
			for (int j = 0; j < K.rows() / 2; j++)
			{
				if (!FixedNodeIndices.Contains(j))
				{
					KReduxSubZero.Add(K(2 * i, 2 * j));
					KReduxSubZero.Add(K(2 * i, 2 * j + 1));
					KReduxSubOne.Add(K(2 * i + 1, 2 * j));
					KReduxSubOne.Add(K(2 * i + 1, 2 * j + 1));
				}
			}
			KReduxArray.Add(KReduxSubZero);
			KReduxArray.Add(KReduxSubOne);
		}
	}

	for (int i = 0; i < KReduxArray.Num(); i++)
	{
		for (int j = 0; j < KReduxArray[i].Num(); j++)
		{
			kRedux(i, j) = KReduxArray[i][j];
		}
	}

	return kRedux;
}

MatrixXf AMeshGenerator::GenerateKReduxInverse(MatrixXf kRedux)
{
	MatrixXf kReduxInverse(kRedux.rows(), kRedux.rows());

	UE_LOG(LogTemp, Warning, TEXT("Inverting"));
	kReduxInverse = kRedux.inverse();
	UE_LOG(LogTemp, Warning, TEXT("Done"));


	return kReduxInverse;
}

MatrixXf AMeshGenerator::ReduceMatrixPrime(MatrixXf kReduxInverse)
{
	TArray<TArray<float>> KReduxInversePrimeArray;

	int FixedNodesPassedI = 0;
	int InternalNodesPassed = 0;

	for (int i = 0; i < Nodes.Num(); i++)
	{
		TArray<float> KReduxInversePrimeSubZero;
		TArray<float> KReduxInversePrimeSubOne;

		if (FixedNodeIndices.Contains(i))
		{
			FixedNodesPassedI += 1;
		}
		else if (EdgeNodeIndices.Contains(i))
		{
			EdgeNodeKMatrixMap.Add(i, i - FixedNodesPassedI - InternalNodesPassed);

			int FixedNodesPassedJ = 0;

			for (int j = 0; j < Nodes.Num(); j++)
			{
				if (FixedNodeIndices.Contains(j))
				{
					FixedNodesPassedJ += 1;
				}
				else if (EdgeNodeIndices.Contains(j))
				{
					KReduxInversePrimeSubZero.Add(kReduxInverse(2 * (i - FixedNodesPassedI), 2 * (j - FixedNodesPassedJ)));
					KReduxInversePrimeSubZero.Add(kReduxInverse(2 * (i - FixedNodesPassedI), 2 * (j - FixedNodesPassedJ) + 1));
					KReduxInversePrimeSubOne.Add(kReduxInverse(2 * (i - FixedNodesPassedI) + 1, 2 * (j - FixedNodesPassedJ)));
					KReduxInversePrimeSubOne.Add(kReduxInverse(2 * (i - FixedNodesPassedI) + 1, 2 * (j - FixedNodesPassedJ) + 1));
				}
			}
			KReduxInversePrimeArray.Add(KReduxInversePrimeSubZero);
			KReduxInversePrimeArray.Add(KReduxInversePrimeSubOne);
		}
		else
		{
			InternalNodesPassed += 1;
		}
	}

	MatrixXf kReduxInversePrime(KReduxInversePrimeArray.Num(), KReduxInversePrimeArray.Num());

	for (int i = 0; i < KReduxInversePrimeArray.Num(); i++)
	{
		for (int j = 0; j < KReduxInversePrimeArray.Num(); j++)
		{
			kReduxInversePrime(i, j) = KReduxInversePrimeArray[i][j];
		}
	}

	return kReduxInversePrime;
}

void AMeshGenerator::WritePositions(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (FVector Position : Positions)
	{
		FString PositionFString = FString::SanitizeFloat(Position.X) + "," + FString::SanitizeFloat(Position.Y) + "," + FString::SanitizeFloat(Position.Z);
		string PositionCString(TCHAR_TO_UTF8(*PositionFString));
		file << PositionCString << endl;
	}
	file.close();
}

void AMeshGenerator::WriteTriangles(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (FFEATriangle Triangle : Triangles)
	{
		file << Triangle.Index0 << endl;
		file << Triangle.Index1 << endl;
		file << Triangle.Index2 << endl;
	}
	file.close();
}

void AMeshGenerator::WriteGlobalMatrixInverse(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < KReduxInversePrime.rows(); i++)
	{
		//string MatrixRow = "";
		for (int j = 0; j < KReduxInversePrime.rows(); j++)
		{
			file << KReduxInversePrime(i, j);
			if (j != KReduxInversePrime.rows() - 1)
			{
				file << ",";
			}
			//MatrixRow += KReduxInverse(i, j) + ",";
		}
		file << endl;
	}
	file.close();
}

void AMeshGenerator::WriteFixedNodes(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < FixedNodeIndices.Num(); i++)
	{
		file << FixedNodeIndices[i];
		if (i != FixedNodeIndices.Num() - 1)
		{
			file << ",";
		}
	}
	file << endl;
	file.close();
}

void AMeshGenerator::WriteEdgeNodes(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < EdgeNodeIndices.Num(); i++)
	{
		file << EdgeNodeIndices[i];
		if (i != EdgeNodeIndices.Num() - 1)
		{
			file << ",";
		}
	}
	file << endl;
	file.close();
}

void AMeshGenerator::WriteNodeGridMap(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < NodeGridMap.rows(); i++)
	{
		for (int j = 0; j < NodeGridMap.cols(); j++)
		{
			file << NodeGridMap(i, j);
			if (j != NodeGridMap.cols() - 1)
			{
				file << ",";
			}
		}
		file << endl;
	}
	file.close();
}

void AMeshGenerator::WriteNodeMatrixMap(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (auto pair : EdgeNodeKMatrixMap)
	{
		file << pair.Key;
		file << ",";
		file << pair.Value;
		file << endl;
	}
	file.close();
}

void AMeshGenerator::WriteMetaData(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	file << Ny << endl;
	file << DeltaY << endl;
	file << Nz << endl;
	file << DeltaZ << endl;
	file << ElasticMod << endl;
	file << FocalPoints.Num() << endl;
	file.close();
}

void AMeshGenerator::WriteGlobalMatrix(FString Path)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < KReduxPrime.rows(); i++)
	{
		for (int j = 0; j < KReduxPrime.rows(); j++)
		{
			file << KReduxPrime(i, j);
			if (j != KReduxPrime.rows() - 1)
			{
				file << ",";
			}
		}
		file << endl;
	}
	file.close();
}

TArray<int> AMeshGenerator::GetMeshDensityData()
{
	TArray<int> meshDensityData;
	meshDensityData.Add(Ny);
	meshDensityData.Add(Nz);
	return meshDensityData;
}

TArray<float> AMeshGenerator::GetMeshSpacingData()
{
	TArray<float> meshSpacingData;
	meshSpacingData.Add(DeltaY);
	meshSpacingData.Add(DeltaZ);
	return meshSpacingData;
}

TArray<FVector> AMeshGenerator::GetNodePositions()
{
	TArray<FVector> nodePositions;
	for (int i = 0; i < Nodes.Num(); i++)
	{
		nodePositions.Add(Nodes[i].Position);
	}
	nodePositions.Add(FVector(0.0, (Ny / 2) * DeltaY, (Nz / 2) * DeltaZ));
	return nodePositions;
}

TArray<int32> AMeshGenerator::GetEdgeNodeIndices()
{
	return EdgeNodeIndices;
}

void AMeshGenerator::SetNy(int inputNy)
{
	Ny = inputNy;
}

void AMeshGenerator::SetNz(int inputNz)
{
	Nz = inputNz;
}

void AMeshGenerator::SetMeshName(FString Name)
{
	MeshName = Name;
}

void AMeshGenerator::SetElasticMod(float inputElasticMod)
{
	ElasticMod = inputElasticMod;
}

void AMeshGenerator::ResetMeshGenerator()
{
	CornerNodes.Empty();
	EdgeLines.Empty();
	Nodes.Empty();
	//NodeGridMap.Empty();
	EdgeNodes.Empty();
	EdgeNodeIndices.Empty();
	FixedNodeIndices.Empty();
	Triangles.Empty();
	Positions.Empty();
	EdgeNodeKMatrixMap.Empty();
	CenterPointPosition = FVector(0.0, 0.0, 0.0);
	CornerNodePositions.Empty();
	FocalPoints.Empty();
}

