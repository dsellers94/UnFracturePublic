// Copyright Daniel Sellers 2023

#include "MeshLoadUtility.h"

MeshLoadUtility::MeshLoadUtility()
{
}

MeshLoadUtility::~MeshLoadUtility()
{
}

TArray<FVector> MeshLoadUtility::ReadInNodes(FString NodesFilePath)
{
	FString Path = NodesFilePath;

	TArray<FVector> nodePositions;

	TArray<FString> CSVLines = GetCSVFile(Path);

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);

		nodePositions.Add(FVector(FCString::Atof(*StringArray[0]), FCString::Atof(*StringArray[1]), FCString::Atof(*StringArray[2])));
	}

	return nodePositions;
}

TArray<int32> MeshLoadUtility::ReadInTriangles(FString TrianglesFilePath)
{
	FString Path = TrianglesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	TArray<int32> triangleIndices;

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		triangleIndices.Add(FCString::Atoi(*CSVLines[i]));
	}

	return triangleIndices;
}

MatrixXf MeshLoadUtility::ReadInGlobalMatrixInverse(FString MatrixInverseFilePath)
{
	FString Path = MatrixInverseFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	MatrixXf kInverse(CSVLines.Num() - 1, CSVLines.Num() - 1);

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);
		for (int j = 0; j < StringArray.Num(); j++)
		{
			kInverse(i, j) = FCString::Atof(*StringArray[j]);
		}
	}

	return kInverse;
}

TArray<int32> MeshLoadUtility::ReadInFixedNodes(FString FixedNodesFilePath)
{
	FString Path = FixedNodesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	TArray<int32> fixedNodes;

	FString aString = CSVLines[0];
	TArray<FString> StringArray = {};
	aString.ParseIntoArray(StringArray, TEXT(","), true);

	for (int i = 0; i < StringArray.Num(); i++)
	{
		fixedNodes.Add(FCString::Atoi(*StringArray[i]));
	}

	return fixedNodes;
}

TArray<int32> MeshLoadUtility::ReadInEdgeNodes(FString EdgeNodesFilePath)
{
	FString Path = EdgeNodesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	TArray<int32> edgeNodes;

	FString aString = CSVLines[0];
	TArray<FString> StringArray = {};
	aString.ParseIntoArray(StringArray, TEXT(","), true);

	for (int i = 0; i < StringArray.Num(); i++)
	{
		edgeNodes.Add(FCString::Atoi(*StringArray[i]));
	}

	return edgeNodes;
}

TMap<int32, int32> MeshLoadUtility::ReadInNodeMatrixMap(FString NodeMatrixMapFilePath)
{
	FString Path = NodeMatrixMapFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	TMap<int32, int32> nodeMatrixMap;

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);

		nodeMatrixMap.Add(FCString::Atoi(*StringArray[0]), FCString::Atoi(*StringArray[1]));
	}

	return nodeMatrixMap;
}

MatrixXf MeshLoadUtility::ReadInNodeGridMap(FString NodeGridMapFilePath)
{
	FString Path = NodeGridMapFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	//Get first line as array in order to calculate matrix size
	FString firstString = CSVLines[0];
	TArray<FString> firstStringArray = {};
	firstString.ParseIntoArray(firstStringArray, TEXT(","), true);

	MatrixXf nodeGridMap(CSVLines.Num() - 1, firstStringArray.Num());

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);
		for (int j = 0; j < StringArray.Num(); j++)
		{
			nodeGridMap(i, j) = FCString::Atof(*StringArray[j]);
		}
	}

	return nodeGridMap;
}

void MeshLoadUtility::ReadInMetaData(
	FString MetaDataFilePath, 
	int& ny, 
	float& deltaY, 
	int& nz, 
	float& deltaZ, 
	float& elasticModulus,
	int& focalPointCount)
{
	FString Path = MetaDataFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	ny = FCString::Atof(*CSVLines[0]);
	deltaY = FCString::Atof(*CSVLines[1]);
	nz = FCString::Atof(*CSVLines[2]);
	deltaZ = FCString::Atof(*CSVLines[3]);
	elasticModulus = FCString::Atof(*CSVLines[4]);
	focalPointCount = FCString::Atof(*CSVLines[5]);
}

MatrixXf MeshLoadUtility::ReadInGlobalMatrix(FString MatrixFilePath)
{
	FString Path = MatrixFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	MatrixXf K(CSVLines.Num() - 1, CSVLines.Num() - 1);

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);
		for (int j = 0; j < StringArray.Num(); j++)
		{
			K(i, j) = FCString::Atof(*StringArray[j]);
		}
	}

	return K;
}

TArray<FString> MeshLoadUtility::GetCSVFile(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	if (FPaths::FileExists(CsvFile))
	{
		FFileHelper::LoadANSITextFileToStrings(*CsvFile, NULL, Lines);
	}

	return Lines;
}

bool MeshLoadUtility::CheckValidMeshFile(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	if (FPaths::FileExists(CsvFile))
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Valid Mesh Data Found"));
		return false;
	}
}

FString MeshLoadUtility::LoadJsonFile(FString filePath)
{
	FString JsonFile = FPaths::ProjectContentDir() + filePath;

	FString jsonStringFromFile;

	if (FPaths::FileExists(JsonFile))
	{
		FFileHelper::LoadFileToString(jsonStringFromFile, *JsonFile, FFileHelper::EHashOptions::None);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Json File Does Not Exist"));
	}

	return jsonStringFromFile;
}

TSharedPtr<FJsonObject> MeshLoadUtility::DeserializeJsonStringToObject(FString jsonString)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(*jsonString);

	TSharedPtr<FJsonObject> jsonObject;

	if (FJsonSerializer::Deserialize(Reader, jsonObject))
	{
		UE_LOG(LogTemp, Warning, TEXT("Json Object Loaded"));
	}

	return jsonObject;
}

TArray<TSharedPtr<FJsonValue>> MeshLoadUtility::GetMeshDataObjectArray(TSharedPtr<FJsonObject> jsonObject)
{
	TArray<TSharedPtr<FJsonValue>> meshDataObjectArray;
	if (jsonObject.IsValid())
	{
		meshDataObjectArray = jsonObject->GetArrayField("Meshes");
	}

	return meshDataObjectArray;
}

TSharedPtr<FJsonValue> MeshLoadUtility::CreateMeshDataArrayElement(
	FString meshName,
	TArray<FVector> cornerNodes,
	TArray<FVector> fixedNodeBoxes,
	TArray<int32> meshDensity,
	float elasticMod,
	TArray<FVector> focalPoints)
{
	TSharedPtr<FJsonObject> MeshDataJsonObject = MakeShareable(new FJsonObject);
	TSharedPtr<FJsonObject> EnclosingJsonObject = MakeShareable(new FJsonObject);

	MeshDataJsonObject->SetStringField("MeshName", meshName);

	TArray<TSharedPtr<FJsonValue>> CornerNodesPtrArray;
	for (int i = 0; i < cornerNodes.Num(); i++)
	{
		TSharedPtr<FJsonValue> XValue = MakeShareable(new FJsonValueNumber(cornerNodes[i].X));
		TSharedPtr<FJsonValue> YValue = MakeShareable(new FJsonValueNumber(cornerNodes[i].Y));
		TSharedPtr<FJsonValue> ZValue = MakeShareable(new FJsonValueNumber(cornerNodes[i].Z));
		CornerNodesPtrArray.Add(XValue);
		CornerNodesPtrArray.Add(YValue);
		CornerNodesPtrArray.Add(ZValue);
	}
	MeshDataJsonObject->SetArrayField("CornerNodes", CornerNodesPtrArray);

	TArray<TSharedPtr<FJsonValue>> FixedNodeBoxesPtrArray;
	for (int i = 0; i < fixedNodeBoxes.Num(); i++)
	{
		TSharedPtr<FJsonValue> XValue = MakeShareable(new FJsonValueNumber(fixedNodeBoxes[i].X));
		TSharedPtr<FJsonValue> YValue = MakeShareable(new FJsonValueNumber(fixedNodeBoxes[i].Y));
		TSharedPtr<FJsonValue> ZValue = MakeShareable(new FJsonValueNumber(fixedNodeBoxes[i].Z));
		FixedNodeBoxesPtrArray.Add(XValue);
		FixedNodeBoxesPtrArray.Add(YValue);
		FixedNodeBoxesPtrArray.Add(ZValue);
	}
	MeshDataJsonObject->SetArrayField("FixedNodeBoxes", FixedNodeBoxesPtrArray);

	TArray<TSharedPtr<FJsonValue>> MeshDensityPtrArray;
	for (int i = 0; i < meshDensity.Num(); i++)
	{
		TSharedPtr<FJsonValue> DensityValue = MakeShareable(new FJsonValueNumber(meshDensity[i]));
		MeshDensityPtrArray.Add(DensityValue);
	}
	MeshDataJsonObject->SetArrayField("MeshDensity", MeshDensityPtrArray);

	MeshDataJsonObject->SetNumberField("ElasticMod", elasticMod);

	TArray<TSharedPtr<FJsonValue>> FocalPointPtrArray;
	for (int i = 0; i < focalPoints.Num(); i++)
	{
		TSharedPtr<FJsonValue> XValue = MakeShareable(new FJsonValueNumber(focalPoints[i].X));
		TSharedPtr<FJsonValue> YValue = MakeShareable(new FJsonValueNumber(focalPoints[i].Y));
		TSharedPtr<FJsonValue> ZValue = MakeShareable(new FJsonValueNumber(focalPoints[i].Z));
		FocalPointPtrArray.Add(XValue);
		FocalPointPtrArray.Add(YValue);
		FocalPointPtrArray.Add(ZValue);
	}
	MeshDataJsonObject->SetArrayField("FocalPoints", FocalPointPtrArray);

	EnclosingJsonObject->SetObjectField("MeshDataObject", MeshDataJsonObject);

	TMap<FString, TSharedPtr<FJsonValue>> MeshDataObjectMap = EnclosingJsonObject->Values;

	TSharedPtr<FJsonValue> ExtractedObjectAsValue = MeshDataObjectMap["MeshDataObject"];
	
	return ExtractedObjectAsValue;
}

void MeshLoadUtility::AppendJsonObjectToArray(TSharedPtr<FJsonValue> jsonArrayElementToAppend, FString FileBaseName)
{
	FString fileName = FileBaseName + ".json";
	FString filePath = "Files/" + fileName;
	FString JsonStringFromFile = LoadJsonFile(filePath);
	TSharedPtr<FJsonObject> JsonObjectFromFile = DeserializeJsonStringToObject(JsonStringFromFile);
	TArray<TSharedPtr<FJsonValue>> MeshDataObjectArray = GetMeshDataObjectArray(JsonObjectFromFile);

	MeshDataObjectArray.Add(jsonArrayElementToAppend);

	FString JsonStringToWrite;
	TSharedPtr<FJsonObject> JsonObjectToWrite; 
	JsonObjectToWrite = MakeShareable(new FJsonObject);
	JsonObjectToWrite->SetArrayField("Meshes", MeshDataObjectArray);

	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonStringToWrite);

	FJsonSerializer::Serialize(JsonObjectToWrite.ToSharedRef(), Writer);

	FString JsonFile = FPaths::ProjectContentDir() + filePath;
	if (FFileHelper::SaveStringToFile(JsonStringToWrite, *JsonFile))
	{
		UE_LOG(LogTemp, Warning, TEXT("Write Successful"));
	}
}

FCompressableData MeshLoadUtility::GetCompressableDataByMeshName(FString MeshName)
{
	FCompressableData CompressableData = FCompressableData();

	CompressableData.NodePositions = ReadInNodes("Files/" + MeshName + "_positions.csv");
	CompressableData.TriangleIndices = ReadInTriangles("Files/" + MeshName + "_triangles.csv");
	CompressableData.KInverse = ReadInGlobalMatrixInverse("Files/" + MeshName + "_KReduxInversePrime.csv");
	CompressableData.K = ReadInGlobalMatrix("Files/" + MeshName + "_KReduxPrime.csv");
	CompressableData.FixedNodes = ReadInFixedNodes("Files/" + MeshName + "_FixedNodes.csv");
	CompressableData.EdgeNodes = ReadInEdgeNodes("Files/" + MeshName + "_EdgeNodes.csv");
	CompressableData.NodeMatrixMap = ReadInNodeMatrixMap("Files/" + MeshName + "_EdgeNodeKMatrixMap.csv");
	CompressableData.NodeGridMap = ReadInNodeGridMap("Files/" + MeshName + "_NodeGridMap.csv");
	int Ny;
	int Nz;
	float DeltaY;
	float DeltaZ;
	float ElasticModulus;
	int FocalPointCount;
	ReadInMetaData("Files/" + MeshName + "_MetaData.csv", Ny, DeltaY, Nz, DeltaZ, ElasticModulus, FocalPointCount);
	CompressableData.Ny = Ny;
	CompressableData.Nz = Nz;
	CompressableData.DeltaY = DeltaY;
	CompressableData.DeltaZ = DeltaZ;
	CompressableData.ElasticModulus = ElasticModulus;
	CompressableData.FocalPointCount = FocalPointCount;

	return CompressableData;
}