// Copyright Daniel Sellers 2023 - 2024

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include <Eigen/Dense>
#include "CompressableInterface.generated.h"

using Eigen::MatrixXf;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCompressableInterface : public UInterface
{
	GENERATED_BODY()
};

class COMPRESSABLE_API ICompressableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetClosestSingleNode(FVector ForceLocation) = 0;

	virtual TArray<int32> GetEdgeNodes() = 0;

	virtual TArray<FVector> GetNodePositions() = 0;

	virtual TArray<FVector> GetMeshDataPositions() = 0;

	virtual FVector GetCompressableLocation() = 0;

	virtual MatrixXf GetNodeGridMap() = 0;

	virtual TArray<int32> GetClosestNodesSorted(FVector TargetLocation, int NodeCount) = 0;
};
