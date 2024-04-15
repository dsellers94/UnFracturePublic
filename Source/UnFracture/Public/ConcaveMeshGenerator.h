// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "MeshGenerator.h"
#include "ConcaveMeshGenerator.generated.h"

/**
 * 
 */
UCLASS()
class UNFRACTURE_API AConcaveMeshGenerator : public AMeshGenerator
{
	GENERATED_BODY()
	
public:

	AConcaveMeshGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float IntersectTestLineFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int JumpTolerance;

	virtual void GenerateMesh(
		TArray<FEdgeLine> edgeLines,
		int ny,
		int nz,
		float deltaY,
		float deltaZ) override;

	bool IsInsideShape(FVector Position, TArray<FEdgeLine> edgeLines);

	bool IsOnEdgeLine(FVector Position, TArray<FEdgeLine> edgeLines);

	bool LinesIntersect(FVector LineOneStart, FVector LineOneEnd, FVector LineTwoStart, FVector LineTwoEnd);

	FString Orientation(FVector P, FVector Q, FVector R);

	virtual void GeneratePhaseOne(TArray<FVector> inputCornerNodePositions) override;

	virtual void GeneratePhaseTwo() override;

	void GenerateTrianglesFromFocalPoints();

	void ExtractPositionsNoCenter();

	float GetAbsoluteRotation(FVector Vector, FVector referenceAxisNormal);

	UFUNCTION()
	void SetupFocalPoints();

	UFUNCTION()
	void SortClaimedNodesByRotation();

	UFUNCTION()
	void AddEdgeNodes();

	virtual void ResetMeshGenerator() override;

protected:
	virtual void BeginPlay() override;

};



/*
* 
* 	void GenerateTrianglesFromTwoFocalPoints();

	bool FindSharedPointIndices(
		FVector focalPointOnePosition, 
		FVector focalPointTwoPosition, 
		FVector focalSeparationVector,
		int32& SharedPointOneIndex,
		int32& SahredPointTwoIndex);

void AConcaveMeshGenerator::GenerateTrianglesFromTwoFocalPoints()
{
	TMap<int32, float> IndexRotationMap;

	FVector FocalPointOnePosition = FocalPoints[0].Position;
	FVector FocalPointTwoPosition = FocalPoints[1].Position;

	FVector ReferenceAxisNormal = FVector(0.0, 1.0, 0.0);

	FVector FocalSeparationVector = FocalPointTwoPosition - FocalPointOnePosition;

	int32 SharedPointOneIndex;
	int32 SharedPointTwoIndex;

	int32 FocalPointOneStartIndex;
	int32 FocalPointOneStopIndex;
	int32 FocalPointTwoStartIndex;
	int32 FocalPointTwoStopIndex;

	int32 FocalPointOnePositionIndex = Nodes[Nodes.Num() - 1].Index + 1;
	int32 FocalPointTwoPositionIndex = Nodes[Nodes.Num() - 1].Index + 2;

	if (!FindSharedPointIndices(FocalPointOnePosition, FocalPointTwoPosition, FocalSeparationVector, SharedPointOneIndex, SharedPointTwoIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Shared Points Not Found - Aborting Mesh Generation"));
		return;
	}

	FVector SeparationVectorOne = Nodes[SharedPointOneIndex].Position - FocalPointOnePosition;
	FVector SeparationVectorTwo = Nodes[SharedPointTwoIndex].Position - FocalPointOnePosition;

	float FocalSeparationVectorRotation = GetAbsoluteRotation(FocalSeparationVector, ReferenceAxisNormal);
	float SeparationVectorOneRotation = GetAbsoluteRotation(SeparationVectorOne, ReferenceAxisNormal);
	float SeparationVectorTwoRotation = GetAbsoluteRotation(SeparationVectorTwo, ReferenceAxisNormal);

	if ((SeparationVectorOneRotation > FocalSeparationVectorRotation
		&& SeparationVectorTwoRotation > FocalSeparationVectorRotation)
		|| (SeparationVectorOneRotation < FocalSeparationVectorRotation
			&& SeparationVectorTwoRotation < FocalSeparationVectorRotation))
	{
		if (SeparationVectorTwoRotation > SeparationVectorOneRotation + PI)
		{
			FocalPointOneStartIndex = SharedPointOneIndex;
		}
		else
		{
			FocalPointOneStartIndex = SharedPointTwoIndex;
		}
	}
	else
	{
		if (SeparationVectorOneRotation > FocalSeparationVectorRotation)
		{
			FocalPointOneStartIndex = SharedPointOneIndex;
		}
		else
		{
			FocalPointOneStartIndex = SharedPointTwoIndex;
		}
	}

	if (FocalPointOneStartIndex == SharedPointOneIndex)
	{
		FocalPointOneStopIndex = SharedPointTwoIndex;
		FocalPointTwoStartIndex = SharedPointTwoIndex;
		FocalPointTwoStopIndex = SharedPointOneIndex;
	}
	else
	{
		FocalPointOneStopIndex = SharedPointOneIndex;
		FocalPointTwoStartIndex = SharedPointOneIndex;
		FocalPointTwoStopIndex = SharedPointTwoIndex;
	}

	float FocalPointOneStartRotation = GetAbsoluteRotation(Nodes[FocalPointOneStartIndex].Position - FocalPointOnePosition, ReferenceAxisNormal);
	float FocalPointOneStopRotation = GetAbsoluteRotation(Nodes[FocalPointOneStopIndex].Position - FocalPointOnePosition, ReferenceAxisNormal);
	float FocalPointTwoStartRotation = GetAbsoluteRotation(Nodes[FocalPointTwoStartIndex].Position - FocalPointTwoPosition, ReferenceAxisNormal);
	float FocalPointTwoStopRotation = GetAbsoluteRotation(Nodes[FocalPointTwoStopIndex].Position - FocalPointTwoPosition, ReferenceAxisNormal);

	//Here is where we will actually check each edge node SET against the start and stop rotations once we figure out how
	//the fuck to do this.

	
	for (int EdgeNodeIndex : EdgeNodeIndices)
	{
		FVector SeparationVector = Nodes[EdgeNodeIndex].Position - CenterPointPosition;
		float VectorDot = (SeparationVector.Y * ReferenceAxisNormal.Y + SeparationVector.Z * ReferenceAxisNormal.Z) / SeparationVector.Length();
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

bool AConcaveMeshGenerator::FindSharedPointIndices(
	FVector focalPointOnePosition,
	FVector focalPointTwoPosition,
	FVector focalSeparationVector,
	int32& sharedPointOneIndex,
	int32& sharedPointTwoIndex)
{
	bool SharedPointOneFound = false;
	bool SharedPointTwoFound = false;

	for (FFEANode EdgeNode : EdgeNodes)
	{
		FVector NodeFocalPointOneSeparationVector = focalPointOnePosition - EdgeNode.Position;
		FVector NodeFocalPointTwoSeparationVector = focalPointTwoPosition - EdgeNode.Position;

		if (
			NodeFocalPointOneSeparationVector.Length() < focalSeparationVector.Length()
			&& NodeFocalPointTwoSeparationVector.Length() < focalSeparationVector.Length())
		{
			for (FEdgeLine EdgeLine : EdgeLines)
			{
				if (!LinesIntersect(focalPointOnePosition, EdgeNode.Position, EdgeLine.StartPoint, EdgeLine.EndPoint)
					&& !LinesIntersect(focalPointTwoPosition, EdgeNode.Position, EdgeLine.StartPoint, EdgeLine.EndPoint))
				{
					if (!SharedPointOneFound)
					{
						FString OrientationCheck = Orientation(focalPointOnePosition, focalPointTwoPosition, EdgeNode.Position);
						if (OrientationCheck == "CCW")
						{
							sharedPointOneIndex = EdgeNode.Index;
							SharedPointOneFound = true;
						}
					}
					if (!SharedPointTwoFound)
					{
						FString OrientationCheck = Orientation(focalPointOnePosition, EdgeNode.Position, focalPointTwoPosition);
						if (OrientationCheck == "CCW")
						{
							sharedPointTwoIndex = EdgeNode.Index;
							SharedPointTwoFound = true;
						}
					}
				}
			}
		}

		if (SharedPointOneFound && SharedPointTwoFound)
		{
			return true;
		}

	}
	UE_LOG(LogTemp, Warning, TEXT("Failed To Find Acceptable Shared Points"));
	return false;
}
*/