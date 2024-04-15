// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeshGenerator.h"
#include "Json.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Templates/SharedPointer.h"
#include "MeshLoadUtility.h"
#include "AutoGenerator.generated.h"

UCLASS()
class UNFRACTURE_API AAutoGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAutoGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	FString GenerationDataFile;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMeshGenerator> MeshGeneratorClass;

	UPROPERTY()
	AMeshGenerator* SpawnedMeshGenerator;

	TArray<FString> PreExistingMeshNames;

	FString JsonStringFromFile;

	TSharedPtr<FJsonObject> JsonObjectFromFile;

	TMap<FString, TSharedPtr<FJsonValue>> JsonValues;

	TArray<TSharedPtr<FJsonValue>> MeshDataObjectArray;

	FString JsonStringToWrite;

	TSharedPtr<FJsonObject> JsonObjectToWrite;

	UFUNCTION(BlueprintCallable)
	void SpawnAndGenerate();

	void ExtractJsonValues(TSharedPtr<FJsonObject> jsonObject);

	UFUNCTION(BlueprintCallable)
	void GenerateAllMeshesFromJson(FString fileName);

	void SpawnMeshGenerator();

	TArray<FVector> ParsePtrArrayToVectorArray(TArray<TSharedPtr<FJsonValue>> inPtrArray);

	bool MeshDataAlreadyExists(FString MeshName);

	UFUNCTION(BlueprintImplementableEvent)
	void GenerationCompleteEvent();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
