// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include <Eigen/Dense>
#include "UnFractureStructs.generated.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

class COMPRESSABLE_API UnFractureStructs
{
public:
	UnFractureStructs();
	~UnFractureStructs();
};

USTRUCT()
struct COMPRESSABLE_API FCompressableData
{
	GENERATED_BODY()

public:

	FCompressableData();

	UPROPERTY()
	TArray<FVector> NodePositions;

	UPROPERTY()
	TArray<int32> TriangleIndices;

	MatrixXf KInverse;

	MatrixXf K;

	UPROPERTY()
	TArray<int32> FixedNodes;

	UPROPERTY()
	TArray<int32> EdgeNodes;

	UPROPERTY()
	TMap<int32, int32> NodeMatrixMap;

	UPROPERTY()
	int Ny;

	UPROPERTY()
	float DeltaY;

	UPROPERTY()
	int Nz;

	UPROPERTY()
	float DeltaZ;

	UPROPERTY()
	float ElasticModulus;

	UPROPERTY()
	int FocalPointCount;

	MatrixXf NodeGridMap;
};

USTRUCT()
struct COMPRESSABLE_API FFEANode
{
	GENERATED_BODY()

public:

	FFEANode();

	UPROPERTY()
		int Index;

	UPROPERTY()
		FVector Position;

	UPROPERTY()
		TArray<int32> NeighborIndices;

	UPROPERTY()
		int32 YIndex = -1;

	UPROPERTY()
		int32 ZIndex = -1;

	UPROPERTY()
		TArray<FVector> NeighborSeparationVectors;

	UPROPERTY()
		bool IsEdgeNode = false;

	UPROPERTY()
		bool IsCornerNode = false;

	bool operator==(const FFEANode& OtherNode) const
	{
		return Index == OtherNode.Index;
	}

	bool operator!=(const FFEANode& OtherNode) const
	{
		return Index != OtherNode.Index;
	}
};

USTRUCT()
struct COMPRESSABLE_API FEdgeLine
{
	GENERATED_BODY()

public:

	FEdgeLine();

	UPROPERTY()
		float m;

	UPROPERTY()
		float b;

	UPROPERTY()
		FVector StartPoint;

	UPROPERTY()
		FVector EndPoint;

	UPROPERTY()
		bool IsHorizontal = false;

	UPROPERTY()
		bool IsVertical = false;

	UPROPERTY()
		bool IsLeftBoundary = false;

	UPROPERTY()
		bool IsRightBoundary = false;

	UPROPERTY()
		bool IsLowerBoundary = false;

	UPROPERTY()
		bool IsUpperBoundary = false;

	void Calculate()
	{
		if (StartPoint.Y == EndPoint.Y) IsVertical = true;

		if (StartPoint.Z == EndPoint.Z) IsHorizontal = true;

		if (!IsVertical)
		{
			m = (EndPoint.Z - StartPoint.Z) / (EndPoint.Y - StartPoint.Y);
			b = StartPoint.Z - StartPoint.Y * m;
		}

		if (IsVertical)
		{
			if (EndPoint.Z < StartPoint.Z)
			{
				IsLeftBoundary = true;
			}
			else
			{
				IsRightBoundary = true;
			}
		}
		else if (IsHorizontal)
		{
			if (EndPoint.Y > StartPoint.Y)
			{
				IsLowerBoundary = true;
			}
			else
			{
				IsUpperBoundary = true;
			}
		}
		else
		{
			if (EndPoint.Z < StartPoint.Z)
			{
				IsLeftBoundary = true;
			}
			else
			{
				IsRightBoundary = true;
			}
			if (EndPoint.Y > StartPoint.Y)
			{
				IsLowerBoundary = true;
			}
			else
			{
				IsUpperBoundary = true;
			}
		}
	}

	void LogEdgeLineResults()
	{
		UE_LOG(LogTemp, Display, TEXT("Vertical: %s Horizontal: %s"), (IsVertical ? TEXT("true") : TEXT("false")), (IsHorizontal ? TEXT("true") : TEXT("false")));
		UE_LOG(LogTemp, Display, TEXT("Left: %s Lower: %s"), (IsLeftBoundary ? TEXT("true") : TEXT("false")), (IsLowerBoundary ? TEXT("true") : TEXT("false")));
		UE_LOG(LogTemp, Display, TEXT("Right: %s Upper: %s"), (IsRightBoundary ? TEXT("true") : TEXT("false")), (IsUpperBoundary ? TEXT("true") : TEXT("false")));
	}
};

USTRUCT()
struct COMPRESSABLE_API FFEATriangle
{
	GENERATED_BODY()

public:
	FFEATriangle();

	FFEATriangle(int32 In0, int32 In1, int32 In2)
	{
		Index0 = In0;
		Index1 = In1;
		Index2 = In2;
	}

	UPROPERTY()
		int32 Index0;

	UPROPERTY()
		int32 Index1;

	UPROPERTY()
		int32 Index2;
};

USTRUCT()
struct COMPRESSABLE_API FFocalPoint
{
	GENERATED_BODY()

public:

	FFocalPoint();

	UPROPERTY()
		FVector Position;

	UPROPERTY()
		int32 Index;

	UPROPERTY()
		bool isOnlyFocalPoint;

	UPROPERTY()
		TArray<int32> ClaimedNodeIndices;

	UPROPERTY()
		TArray<int32> SharedNodeIndices;

	UPROPERTY()
		TArray<int32> ClaimedNodesSortedByRotation;

};