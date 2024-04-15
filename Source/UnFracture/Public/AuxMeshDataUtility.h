// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UNFRACTURE_API AuxMeshDataUtility
{
public:
	AuxMeshDataUtility();
	~AuxMeshDataUtility();

	static void WriteBindableNodes(FString Path, TArray<int32> BindableNodeIndices);

	static TArray<int32> ReadInBindableNodes(FString BindableNodesFilePath);

	static TArray<FString> GetCSVFile(FString Path);

	static bool CheckValidMeshFile(FString Path);
};
