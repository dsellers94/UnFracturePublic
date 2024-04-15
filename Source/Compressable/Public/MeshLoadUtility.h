// Copyright Daniel Sellers 2023

#pragma once

#include <Eigen/Dense>
#include "CoreMinimal.h"
#include "Json.h"
#include "UnfractureStructs.h"

using Eigen::VectorXf;
using Eigen::MatrixXf;

class COMPRESSABLE_API MeshLoadUtility
{
public:
	MeshLoadUtility();
	~MeshLoadUtility();

	static TArray<FVector> ReadInNodes(FString NodesFilePath);

	static TArray<FString> GetCSVFile(FString Path);

	static TArray<int32> ReadInTriangles(FString TrianglesFilePath);

	static MatrixXf ReadInGlobalMatrixInverse(FString MatrixInverseFilePath);

	static TArray<int32> ReadInFixedNodes(FString FixedNodesFilePath);

	static TArray<int32> ReadInEdgeNodes(FString EdgeNodesFilePath);

	static TMap<int32, int32> ReadInNodeMatrixMap(FString NodeMatrixMapFilePath);

	static MatrixXf ReadInNodeGridMap(FString NodeGridMapFilePath);

	static void ReadInMetaData(
		FString MetaDataFilePath, 
		int& ny, 
		float& deltaY, 
		int& nz, 
		float& deltaZ, 
		float& elasticModulus,
		int& focalPointCount);

	static MatrixXf ReadInGlobalMatrix(FString MatrixFilePath);

	static bool CheckValidMeshFile(FString Path);

	static FString LoadJsonFile(FString filePath);

	static TSharedPtr<FJsonObject> DeserializeJsonStringToObject(FString jsonString);

	static TArray<TSharedPtr<FJsonValue>> GetMeshDataObjectArray(TSharedPtr<FJsonObject> jsonObject);

	static TSharedPtr<FJsonValue> CreateMeshDataArrayElement(
		FString meshName, 
		TArray<FVector> cornerNodes, 
		TArray<FVector> fixedNodeBoxes,
		TArray<int32> meshDensity,
		float elasticMod,
		TArray<FVector> focalPoints);

	static void AppendJsonObjectToArray(TSharedPtr<FJsonValue> jsonArrayElementToAppend, FString FileBaseName);

	static FCompressableData GetCompressableDataByMeshName(FString MeshName);

};
