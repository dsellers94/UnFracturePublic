// Copyright Daniel Sellers 2023


#include "AutoGenerator.h"

// Sets default values
AAutoGenerator::AAutoGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	JsonObjectFromFile = MakeShareable(new FJsonObject);

	JsonObjectToWrite = MakeShareable(new FJsonObject);

	GenerationDataFile = "GenerationData";
}

// Called when the game starts or when spawned
void AAutoGenerator::BeginPlay()
{
	Super::BeginPlay();

}

void AAutoGenerator::SpawnAndGenerate()
{
	SpawnMeshGenerator();

	GenerateAllMeshesFromJson(GenerationDataFile + ".json");
}

void AAutoGenerator::GenerateAllMeshesFromJson(FString fileName)
{
	FString filePath = "Files/" + fileName;
	JsonStringFromFile = MeshLoadUtility::LoadJsonFile(filePath);
	JsonObjectFromFile = MeshLoadUtility::DeserializeJsonStringToObject(JsonStringFromFile);
	MeshDataObjectArray = MeshLoadUtility::GetMeshDataObjectArray(JsonObjectFromFile);

	for (int i = 0; i < MeshDataObjectArray.Num(); i++)
	{
		TSharedPtr<FJsonObject> meshDataJsonObject = MeshDataObjectArray[i]->AsObject();

		FString MeshName = meshDataJsonObject->GetStringField("MeshName");

		if (MeshDataAlreadyExists(MeshName))
		{
			UE_LOG(LogTemp, Warning, TEXT("Mesh data for %s already exists"), *MeshName);
			continue;
		}
		else if (MeshName == "" || MeshName == "MeshName")
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid Mesh Name"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Generating Data for %s"), *MeshName);
		}

		TArray<TSharedPtr<FJsonValue>> CornerNodesPtrArray = meshDataJsonObject->GetArrayField("CornerNodes");

		TArray<FVector> CornerNodesVectorArray = ParsePtrArrayToVectorArray(CornerNodesPtrArray);

		TArray<TSharedPtr<FJsonValue>> FixedNodeBoxesPtrArray = meshDataJsonObject->GetArrayField("FixedNodeBoxes");

		TArray<FVector> FixedNodeBoxesVectorArray = ParsePtrArrayToVectorArray(FixedNodeBoxesPtrArray);

		TArray<TSharedPtr<FJsonValue>> MeshDensityPtrArray = meshDataJsonObject->GetArrayField("MeshDensity");

		float ElasticMod = meshDataJsonObject->GetNumberField("ElasticMod");

		TArray<TSharedPtr<FJsonValue>> FocalPointPtrArray = meshDataJsonObject->GetArrayField("FocalPoints");

		TArray<FVector> FocalPointVectorArray = ParsePtrArrayToVectorArray(FocalPointPtrArray);

		if (SpawnedMeshGenerator)
		{
			UE_LOG(LogTemp, Warning, TEXT("Mesh Generator Found"));

			int NyValue;
			int NzValue;

			if (MeshDensityPtrArray.Num() > 1)
			{
				NyValue = MeshDensityPtrArray[1]->AsNumber();
				NzValue = MeshDensityPtrArray[2]->AsNumber();
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No Mesh Density Found"));
			}

			SpawnedMeshGenerator->SetMeshName(MeshName);
			SpawnedMeshGenerator->SetNy(NyValue);
			SpawnedMeshGenerator->SetNz(NzValue);
			SpawnedMeshGenerator->SetElasticMod(ElasticMod);
			SpawnedMeshGenerator->GeneratePhaseOne(CornerNodesVectorArray);
			for (int j = 0; j < FixedNodeBoxesVectorArray.Num(); j += 2)
			{
				SpawnedMeshGenerator->SetFixedNodesInBox(FixedNodeBoxesVectorArray[j], FixedNodeBoxesVectorArray[j + 1]);
			}
			for (int j = 0; j < FocalPointVectorArray.Num(); j++)
			{
				SpawnedMeshGenerator->AddFocalPoint(FocalPointVectorArray[j]);
			}
			SpawnedMeshGenerator->GeneratePhaseTwo();
			SpawnedMeshGenerator->WriteAllToCSV();
		}
		SpawnedMeshGenerator->ResetMeshGenerator();
	}
	GenerationCompleteEvent();

}

// Called every frame
void AAutoGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAutoGenerator::ExtractJsonValues(TSharedPtr<FJsonObject> jsonObject)
{
	JsonValues = jsonObject->Values;
}

void AAutoGenerator::SpawnMeshGenerator()
{
	FActorSpawnParameters SpawnInfo;
	FVector Location(0.0, 0.0, 0.0);
	FRotator Rotation(0.0, 0.0, 0.0);
	
	SpawnedMeshGenerator = GetWorld()->SpawnActor<AMeshGenerator>(MeshGeneratorClass, Location, Rotation, SpawnInfo);
}

TArray<FVector> AAutoGenerator::ParsePtrArrayToVectorArray(TArray<TSharedPtr<FJsonValue>> inPtrArray)
{
	TArray<FVector> VectorArray;

	for (int i = 0; i < inPtrArray.Num(); i += 3)
	{
		FVector Vector = FVector(inPtrArray[i]->AsNumber(), inPtrArray[i + 1]->AsNumber(), inPtrArray[i + 2]->AsNumber());
		VectorArray.Add(Vector);
	}

	return VectorArray;
}

bool AAutoGenerator::MeshDataAlreadyExists(FString MeshName)
{
	FString Path = "Files/" + MeshName + "_positions.csv";

	FString PositionsFile = FPaths::ProjectContentDir() + Path;

	if (FPaths::FileExists(PositionsFile))
	{
		return true;
	}
	else
	{
		return false;
	}
}

