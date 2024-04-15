// Copyright Daniel Sellers 2023


#include "AuxMeshDataUtility.h"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

AuxMeshDataUtility::AuxMeshDataUtility()
{
}

AuxMeshDataUtility::~AuxMeshDataUtility()
{
}

void AuxMeshDataUtility::WriteBindableNodes(FString Path, TArray<int32> BindableNodeIndices)
{
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	string CsvFileCString(TCHAR_TO_UTF8(*CsvFile));
	ofstream file;
	file.open(CsvFileCString);
	for (int i = 0; i < BindableNodeIndices.Num(); i++)
	{
		file << BindableNodeIndices[i];
		if (i != BindableNodeIndices.Num() - 1)
		{
			file << ",";
		}
	}
	file << endl;
	file.close();
}

TArray<int32> AuxMeshDataUtility::ReadInBindableNodes(FString BindableNodesFilePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Path = BindableNodesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	TArray<int32> bindableNodes;

	FString aString = CSVLines[0];
	TArray<FString> StringArray = {};
	aString.ParseIntoArray(StringArray, TEXT(","), true);

	for (int i = 0; i < StringArray.Num(); i++)
	{
		bindableNodes.Add(FCString::Atoi(*StringArray[i]));
	}

	return bindableNodes;
}

TArray<FString> AuxMeshDataUtility::GetCSVFile(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	if (FPaths::FileExists(CsvFile))
	{
		UE_LOG(LogTemp, Display, TEXT("File %s exists"), *CsvFile);
		FFileHelper::LoadANSITextFileToStrings(*CsvFile, NULL, Lines);
	}

	return Lines;
}

bool AuxMeshDataUtility::CheckValidMeshFile(FString Path)
{
	TArray<FString> Lines;
	FString CsvFile = FPaths::ProjectContentDir() + Path;
	if (FPaths::FileExists(CsvFile))
	{
		return true;
	}
	else
	{
		return false;
	}
}