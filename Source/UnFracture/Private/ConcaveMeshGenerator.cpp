// Copyright Daniel Sellers 2023


#include "ConcaveMeshGenerator.h"
#include "DrawDebugHelpers.h"

AConcaveMeshGenerator::AConcaveMeshGenerator()
{
	IntersectTestLineFactor = 0.9;
	JumpTolerance = 5;
}

void AConcaveMeshGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void AConcaveMeshGenerator::GeneratePhaseOne(TArray<FVector> inputCornerNodePositions)
{
	CreateCornerNodesFromPositions(inputCornerNodePositions);
	DefineEdgesAndGrid(CornerNodes, Ny, Nz);
	GenerateMesh(EdgeLines, Ny, Nz, DeltaY, DeltaZ);
	GenerateMeshConnections(NodeGridMap, Ny, Nz);
	GenerateSeparationVectors();
	ExtractEdgeNodeIndices();
	AddEdgeNodes();
	UE_LOG(LogTemp, Display, TEXT("Generated Mesh From Corners, Waiting for Fixed Nodes"));
}

void AConcaveMeshGenerator::GeneratePhaseTwo()
{
	ExtractPositionsNoCenter();
	GenerateTrianglesFromFocalPoints();
	MatrixXf KGlobal = GenerateGlobalMatrix(ElasticMod, Area);
	KRedux = ReduceMatrix(KGlobal);
	KReduxInverse = GenerateKReduxInverse(KRedux);
	KReduxInversePrime = ReduceMatrixPrime(KReduxInverse);
	KReduxPrime = ReduceMatrixPrime(KRedux);
}

void AConcaveMeshGenerator::GenerateMesh(
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
			FFEANode NewNode = FFEANode();
			NewNode.Index = IndexCount;
			NewNode.YIndex = i;
			NewNode.ZIndex = j;

			NewNode.Position = FVector(0.0, i * deltaY, j * deltaZ);

			bool ValidCandidate = false;
			
			//bool ValidCandidate = IsInsideShape(NewNode.Position, EdgeLines);

			if (IsInsideShape(NewNode.Position, EdgeLines)) ValidCandidate = true;

			if (IsOnEdgeLine(NewNode.Position, EdgeLines)) ValidCandidate = true;

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

void AConcaveMeshGenerator::GenerateTrianglesFromFocalPoints()
{
	SetupFocalPoints();
	SortClaimedNodesByRotation();

	//Perform triangulation by looping through focal points and their sorted claimed nodes
	for (int i = 0; i < FocalPoints.Num(); i++)
	{
		for (int j = 0; j < FocalPoints[i].ClaimedNodesSortedByRotation.Num() - 1; j++)
		{
			int32 CurrentNodeIndex = FocalPoints[i].ClaimedNodesSortedByRotation[j];
			int32 NextNodeIndex = FocalPoints[i].ClaimedNodesSortedByRotation[j + 1];

			float NodeSeparation = FVector::Distance(Nodes[CurrentNodeIndex].Position, Nodes[NextNodeIndex].Position);

			if (NodeSeparation > JumpTolerance * DeltaY && NodeSeparation > JumpTolerance * DeltaZ)
			{
				continue;
			}
			FFEATriangle NewTriangle = FFEATriangle(CurrentNodeIndex, NextNodeIndex, FocalPoints[i].Index);
			Triangles.Add(NewTriangle);
		}

		//Handle triangulating with the last node in each set of sorted nodes. 
		int ClaimedNodeListLength = FocalPoints[i].ClaimedNodesSortedByRotation.Num();
		if (ClaimedNodeListLength < 1)
		{
			UE_LOG(LogTemp, Error, TEXT("No Claimed Nodes Found; Aborting"));
			return;
		}
		int32 LastIndex = FocalPoints[i].ClaimedNodesSortedByRotation[ClaimedNodeListLength - 1];
		int32 FirstIndex = FocalPoints[i].ClaimedNodesSortedByRotation[0];
		float NodeSeparation = FVector::Distance(Nodes[LastIndex].Position, Nodes[FirstIndex].Position);
		if (NodeSeparation < JumpTolerance * DeltaY && NodeSeparation < JumpTolerance * DeltaZ)
		{
			FFEATriangle NewTriangle = FFEATriangle(LastIndex, FirstIndex, FocalPoints[i].Index);
			Triangles.Add(NewTriangle);
		}

		//Finally, triangulate between focal points:
		//If the orientation of (this FP, The next FP, SP 0) is CCW, form a triangle from it, and another
		//Triangle from (this FP, SP 1 and the next FP).
		//If not, then form a triangle from (This FP, the next FP and SP1) and another from (This FP, SP 0, the next FP)
		if (i != FocalPoints.Num() - 1)
		{

			if (Orientation(
				FocalPoints[i].Position,
				FocalPoints[i + 1].Position,
				Nodes[FocalPoints[i].SharedNodeIndices[0]].Position)
				== "CCW")
			{
				FFEATriangle UpperTriangle = FFEATriangle(
					FocalPoints[i].Index,
					FocalPoints[i + 1].Index,
					FocalPoints[i].SharedNodeIndices[0]);
				
				Triangles.Add(UpperTriangle);

				FFEATriangle LowerTriangle = FFEATriangle(
					FocalPoints[i].Index,
					FocalPoints[i].SharedNodeIndices[1],
					FocalPoints[i + 1].Index);
				
				Triangles.Add(LowerTriangle);
			}
			else
			{
				FFEATriangle UpperTriangle = FFEATriangle(
					FocalPoints[i].Index,
					FocalPoints[i + 1].Index,
					FocalPoints[i].SharedNodeIndices[1]);

				Triangles.Add(UpperTriangle);

				FFEATriangle LowerTriangle = FFEATriangle(
					FocalPoints[i].Index,
					FocalPoints[i].SharedNodeIndices[0],
					FocalPoints[i + 1].Index);

				Triangles.Add(LowerTriangle);
			}
		}

		//Add the Focal Point position to the Positions array
		Positions.Add(FocalPoints[i].Position);
	}


}

void AConcaveMeshGenerator::SetupFocalPoints()
{
	for (int i = 0; i < FocalPoints.Num(); i++)
	{
		//Get good index values for each focal point for triangulation
		FocalPoints[i].Index = Nodes[Nodes.Num() - 1].Index + 1 + i;
		//Check if each node meets criteria for claiming by each focal point: Has line of sight, is closest focal point
		for (FFEANode EdgeNode : EdgeNodes)
		{
			bool NodeIsCandidate = true;
			FVector NodeFocalSeparationVector = EdgeNode.Position - FocalPoints[i].Position;
			FVector NodeFocalTestLine = NodeFocalSeparationVector * IntersectTestLineFactor;

			//If this focal point does not have line of sight to this node, mark it as false and break the loop
			for (FEdgeLine EdgeLine : EdgeLines)
			{
				if (LinesIntersect(
					EdgeLine.StartPoint,
					EdgeLine.EndPoint,
					FocalPoints[i].Position,
					FocalPoints[i].Position + NodeFocalTestLine))
				{
					NodeIsCandidate = false;
					break;
				}
			}

			//If this focal point is not the closest to this node, check if the closer focal point has line of sight to this node, if so, mark false and break loop
			for (int j = 0; j < FocalPoints.Num(); j++)
			{
				if (i == j) continue;
				
				FVector RivalFocalSeparationVector = EdgeNode.Position - FocalPoints[j].Position;
				FVector RivalFocalTestLine = RivalFocalSeparationVector * IntersectTestLineFactor;

				if (RivalFocalSeparationVector.Length() < NodeFocalSeparationVector.Length()
					|| (RivalFocalSeparationVector.Length() == NodeFocalSeparationVector.Length() && i < j))
				{
					
					bool RivalFocalPointWillSteal = true;

					for (FEdgeLine EdgeLine : EdgeLines)
					{
						if (LinesIntersect(
							EdgeLine.StartPoint,
							EdgeLine.EndPoint,
							FocalPoints[j].Position,
							FocalPoints[j].Position + RivalFocalTestLine))
						{
							RivalFocalPointWillSteal = false;
							break;
						}
					}

					if (RivalFocalPointWillSteal)
					{
						NodeIsCandidate = false;
						break;
					}
				}
			}

			if (NodeIsCandidate)
			{
				FocalPoints[i].ClaimedNodeIndices.Add(EdgeNode.Index);
			}
		}
	}


	//Now we've looped through and claimed nodes, we need to loop again to find shared nodes
	//We will search the claimed nodes for each focal to find two with neighbors that belong to the next focal
	//Then add that neighboring node to the next focal's claimed nodes

	if (FocalPoints.Num() == 1) return; //Don't look for shared nodes if there's only one focal point

	for (int i = 0; i < FocalPoints.Num() - 1; i++)
	{
		for (int j = 0; j < FocalPoints[i].ClaimedNodeIndices.Num(); j++)
		{
			int32 ClaimedNodeIndex = FocalPoints[i].ClaimedNodeIndices[j];
			for (int k = 0; k < Nodes[ClaimedNodeIndex].NeighborIndices.Num(); k++)
			{
				int32 NeighborIndex = Nodes[ClaimedNodeIndex].NeighborIndices[k];
				if (FocalPoints[i + 1].ClaimedNodeIndices.Contains(NeighborIndex))
				{
					if (!FocalPoints[i].SharedNodeIndices.Contains(NeighborIndex))
					{
						FocalPoints[i].SharedNodeIndices.Add(ClaimedNodeIndex);
						FocalPoints[i + 1].ClaimedNodeIndices.Add(ClaimedNodeIndex);
					}
				}
			}
		}
		if (FocalPoints[i].SharedNodeIndices.Num() != 2)
		{
			UE_LOG(LogTemp, Warning, TEXT("Focal Point Set Up Has Failed For Focal Point %d, Select New Focal Points"), i);
			UE_LOG(LogTemp, Warning, TEXT("Shared Nodes Found: %d"), FocalPoints[i].SharedNodeIndices.Num());
			for (int j = 0; j < FocalPoints[i].SharedNodeIndices.Num(); j++)
			{
				UE_LOG(LogTemp, Warning, TEXT("Shared Node: %d"), FocalPoints[i].SharedNodeIndices[j])
			}

		}
	}

	for (FEdgeLine EdgeLine : EdgeLines)
	{
		FVector TransformStart = FVector(EdgeLine.StartPoint.Z, EdgeLine.StartPoint.Y, 0);
		FVector TransformEnd = FVector(EdgeLine.EndPoint.Z, EdgeLine.EndPoint.Y, 0);
		DrawDebugLine(GetWorld(), TransformStart, TransformEnd, FColor::Red, true);
	}
}

void AConcaveMeshGenerator::SortClaimedNodesByRotation()
{
	FVector ReferenceAxisNormal = FVector(0.0, 1.0, 0.0);

	for (int i = 0; i < FocalPoints.Num(); i++)
	{
		TMap<int32, float> IndexRotationMap;

		for (int ClaimedNodeIndex : FocalPoints[i].ClaimedNodeIndices)
		{
			FVector SeparationVector = Nodes[ClaimedNodeIndex].Position - FocalPoints[i].Position;
			float Theta = GetAbsoluteRotation(SeparationVector, ReferenceAxisNormal);
			IndexRotationMap.Add(ClaimedNodeIndex, Theta);
		}

		IndexRotationMap.ValueSort([](float A, float B) {
			return A < B;
			});

		for (auto& pair : IndexRotationMap)
		{
			FocalPoints[i].ClaimedNodesSortedByRotation.Add(pair.Key);
		}
	}
}

float AConcaveMeshGenerator::GetAbsoluteRotation(FVector Vector, FVector referenceAxisNormal)
{
	float VectorDot = (Vector.Y * referenceAxisNormal.Y + Vector.Z * referenceAxisNormal.Z) / Vector.Length();
	float Theta = UKismetMathLibrary::Acos(VectorDot);
	if (Vector.Z < 0)
	{
		return 2 * PI - Theta;
	}
	else
	{
		return Theta;
	}
}

bool AConcaveMeshGenerator::IsInsideShape(FVector Position, TArray<FEdgeLine> edgeLines)
{
	int CrossCounter = 0;

	for (FEdgeLine EdgeLine : edgeLines)
	{
		float LineLowPoint = FMath::Min(EdgeLine.StartPoint.Z, EdgeLine.EndPoint.Z);
		float LineHighPoint = FMath::Max(EdgeLine.StartPoint.Z, EdgeLine.EndPoint.Z);
		float LineLeftPoint = FMath::Min(EdgeLine.StartPoint.Y, EdgeLine.EndPoint.Y);
		float LineRightPoint = FMath::Max(EdgeLine.StartPoint.Y, EdgeLine.EndPoint.Y);

		if (Position.Z > LineLowPoint && Position.Z <= LineHighPoint)
		{
			if (Position.Y < LineRightPoint)
			{
				if (EdgeLine.IsVertical)
				{
					CrossCounter++;
				}
				else
				{
					float YIntercept = (Position.Z - EdgeLine.b) / EdgeLine.m;

					if (EdgeLine.IsHorizontal || Position.Y <= YIntercept)
					{
						CrossCounter++;
					}
				}
			}
		}
	}
	if (CrossCounter % 2 == 0)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool AConcaveMeshGenerator::IsOnEdgeLine(FVector Position, TArray<FEdgeLine> edgeLines)
{
	for (FEdgeLine EdgeLine : edgeLines)
	{
		float LineLowPoint = FMath::Min(EdgeLine.StartPoint.Z, EdgeLine.EndPoint.Z);
		float LineHighPoint = FMath::Max(EdgeLine.StartPoint.Z, EdgeLine.EndPoint.Z);
		float LineLeftPoint = FMath::Min(EdgeLine.StartPoint.Y, EdgeLine.EndPoint.Y);
		float LineRightPoint = FMath::Max(EdgeLine.StartPoint.Y, EdgeLine.EndPoint.Y);

		if (Position.Z >= LineLowPoint && Position.Z <= LineHighPoint)
		{
			if (EdgeLine.IsVertical && Position.Y == LineRightPoint) return true;

			if (EdgeLine.IsHorizontal && Position.Y >= LineLeftPoint && Position.Y <= LineRightPoint) return true;

			if (!EdgeLine.IsVertical && !EdgeLine.IsHorizontal && Position.Z == EdgeLine.m * Position.Y + EdgeLine.b) return true;
		}
	}

	return false;
}


bool AConcaveMeshGenerator::LinesIntersect(FVector LineOneStart, FVector LineOneEnd, FVector LineTwoStart, FVector LineTwoEnd)
{
	FString Orientation1 = Orientation(LineOneStart, LineOneEnd, LineTwoStart);
	FString Orientation2 = Orientation(LineOneStart, LineOneEnd, LineTwoEnd);
	FString Orientation3 = Orientation(LineTwoStart, LineTwoEnd, LineOneStart);
	FString Orientation4 = Orientation(LineTwoStart, LineTwoEnd, LineOneEnd);

	if (Orientation1 != Orientation2 && Orientation3 != Orientation4)
	{
		return true;
	}
	else
	{
		return false;
	}
}

FString AConcaveMeshGenerator::Orientation(FVector P, FVector Q, FVector R)
{
	int Result = (Q.Z - P.Z) * (R.Y - Q.Y) - (Q.Y - P.Y) * (R.Z - Q.Z);

	if (Result == 0) return "COLINEAR";

	return (Result > 0) ? "CW" : "CCW";
}

void AConcaveMeshGenerator::ExtractPositionsNoCenter()
{
	for (int i = 0; i < Nodes.Num(); i++)
	{
		Positions.Add(Nodes[i].Position);
	}
}

void AConcaveMeshGenerator::AddEdgeNodes()
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

void AConcaveMeshGenerator::ResetMeshGenerator()
{
	Super::ResetMeshGenerator();
	FocalPoints.Empty();
}
