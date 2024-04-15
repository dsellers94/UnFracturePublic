// Fill out your copyright notice in the Description page of Project Settings.


#include "RMCTestActor.h"
#include "Misc/FileHelper.h"
#include "Containers/UnrealString.h"
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using Eigen::MatrixXd;
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::VectorXd;

// Sets default values
ARMCTestActor::ARMCTestActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	RealtimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	SetRootComponent(RealtimeMeshComponent);

	//RealtimeMesh = RealtimeMeshComponent->InitializeRealtimeMesh<URealtimeMeshSimple>();
}

// Called when the game starts or when spawned
void ARMCTestActor::BeginPlay()
{
	Super::BeginPlay();

	RealtimeMesh = NewObject<URealtimeMeshSimple>();
	RealtimeMeshComponent->SetRealtimeMesh(RealtimeMesh);

	CreateTriangle();

	MatrixXf m(2, 2);
	m(0, 0) = 3;
	m(1, 0) = 2.5;
	m(0, 1) = -1;
	m(1, 1) = m(1, 0) + m(0, 1);

	UE_LOG(LogTemp, Display, TEXT("I'm printing inside Eigen?: %f"), m(0, 0));

	VectorXf v(2);
	v(0) = 0;
	v(1) = 0;

	VectorXf p(2);
	p = m * v;

	UE_LOG(LogTemp, Display, TEXT("p(1): %f"), p(1));
}

// Called every frame
void ARMCTestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UpdateTriangle(DeltaTime);

	//UpdateMesh();
}

// This function has become the catch all for actions to take during BeginPlay, we'll create a version of this in the new base class, including the unneeded MeshData 
// properties to remind us to figure out how to handle them later.
void ARMCTestActor::CreateTriangle()
{
	/*ReadInNodes("Files/AngledMesh_positions.csv");
	ReadInTriangles("Files/AngledMesh_triangles.csv");
	KInverse = ReadInGlobalMatrixInverse("Files/AngledMesh_KReduxInverse.csv");
	ReadInFixedNodes("Files/AngledMesh_FixedNodes.csv");*/

	//ForcedNodes = SetContigousForcedNodes(1180, 1160);

	//NOTE: NOT working for x direction forces. KINDA working for y direction forces after switching the global matrix signs in python... for some reason
	//ForceVector = ConstructForceVector(ForcedNodes, FixedNodes, 20.0, -25.0, NodePositions.Num());

	//DisplacementVector = CalculateDisplacementVector();

	//UE_LOG(LogTemp, Display, TEXT("KInverse (0,0): %f"), KInverse(0,0));
	//UE_LOG(LogTemp, Display, TEXT("ForceVector Length: %d"), ForceVector.rows());
	//UE_LOG(LogTemp, Display, TEXT("ForceVector (0): %f"), ForceVector(0));
	//UE_LOG(LogTemp, Display, TEXT("ForceVector (Next To Last Y): %f"), ForceVector(2277));
	//UE_LOG(LogTemp, Display, TEXT("ForceVector (Last Y): %f"), ForceVector(2279));
	//UE_LOG(LogTemp, Display, TEXT("DisplacementVector (0): %f"), DisplacementVector(0));
	//UE_LOG(LogTemp, Display, TEXT("DisplacementVector Length: %d"), DisplacementVector.rows());

	NodePositions.Add(FVector(0.0, 0.0, 0.0));
	NodePositions.Add(FVector(0.0, 100, 0.0));
	NodePositions.Add(FVector(0.0, 0.0, 100));
	NodePositions.Add(FVector(0.0, 100.0, 100.0));

	NodePositions.Add(FVector(0.0, 100.0, 100.0));
	NodePositions.Add(FVector(0.0, 200.0, 100));
	NodePositions.Add(FVector(0.0, 100.0, 200.0));

	TriangleIndices.Add(0);
	TriangleIndices.Add(1);
	TriangleIndices.Add(2);

	TriangleIndices.Add(2);
	TriangleIndices.Add(1);
	TriangleIndices.Add(3);

	TriangleIndices.Add(4);
	TriangleIndices.Add(5);
	TriangleIndices.Add(6);

	TArray<FVector> NormalsBuffer;
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	TArray<FVector> TangentsBuffer;
	//TangentsBuffer.Add(FVector(0, 1, 0));
	//TangentsBuffer.Add(FVector(0, 1, 0));
	//TangentsBuffer.Add(FVector(0, 1, 0));
	//TangentsBuffer.Add(FVector(0, 1, 0));
	//TangentsBuffer.Add(FVector(0, 1, 0));
	//TangentsBuffer.Add(FVector(0, 1, 0));
	TArray<FLinearColor> vertexColorsBuffer;
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	//vertexColorsBuffer.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
	TArray<FVector2D> UV0Buffer;
	UV0Buffer.Add(FVector2D(0, 0));
	UV0Buffer.Add(FVector2D(0.5, 0));
	UV0Buffer.Add(FVector2D(0, 0.5));
	UV0Buffer.Add(FVector2D(0.5, 0.5));
	UV0Buffer.Add(FVector2D(0.5, 0.5));
	UV0Buffer.Add(FVector2D(1, .5));
	UV0Buffer.Add(FVector2D(0.5, 1));
	TArray<FColor> ColorsBuffer;
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));
	//ColorsBuffer.Add(FColor(0.75, 0.75, 0.75, 1.0));

	MeshData.Triangles = TriangleIndices;
	MeshData.Positions = NodePositions; 
	MeshData.Normals = NormalsBuffer;
	MeshData.Tangents = TangentsBuffer;
	MeshData.LinearColors = vertexColorsBuffer;
	MeshData.UV0 = UV0Buffer;
	MeshData.Colors = ColorsBuffer;

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		true);

	// This funtcion call obviously will need to happen on tick, but I want to see if it works just once first
	//UpdateMesh();
}

//This one will need to move to base class
VectorXf ARMCTestActor::CalculateDisplacementVector()
{

	UE_LOG(LogTemp, Display, TEXT("KInverse Length: %d"), KInverse.rows());
	UE_LOG(LogTemp, Display, TEXT("KInverse Depth: %d"), KInverse.cols());
	UE_LOG(LogTemp, Display, TEXT("ForceVector Length: %d"), ForceVector.rows());

	VectorXf displacementVector(KInverse.rows());
	
	//WHAT THE FUCK!!

	displacementVector = KInverse * ForceVector;

	float entry0Result = 0.0f;
	for (int i = 0; i < KInverse.rows(); i++)
	{
		entry0Result += KInverse(0, i) * ForceVector(i);
	}

	UE_LOG(LogTemp, Display, TEXT("entry0Result: %f"), entry0Result);

	float testResult = KInverse(0, 0) * ForceVector(0);

	UE_LOG(LogTemp, Display, TEXT("testResult: %f"), testResult);

	UE_LOG(LogTemp, Display, TEXT("displacementVector Length: %d"), displacementVector.rows());

	return displacementVector;
}

//Note that there is an unfortunate (lazy) coordinate transformation going on here: Python and Eigen are treating the mesh coordinate as X-Y, but the MeshData
//treats them as X-Y-Z with X always 0 by defualt engine orientation, so the mesh only transforms in the Y-Z plane. Therefore X-Y -> Y-Z unless I get unlazy later
//and reorient it. This function will also need to move to base class
void ARMCTestActor::UpdateMesh()
{
	//WRONG: Displacement Vector is not 1:1 with MeshData.Positions - FIX THIS
	int FixedNodesPassed = 0;

	for (int i = 0; i < MeshData.Positions.Num(); i++)
	{
		if (FixedNodes.Contains(i))
		{
			FixedNodesPassed += 1;
		}
		else
		{
			float NewDisplacementX = NodePositions[i].Y + DisplacementVector(2 * (i - FixedNodesPassed));
			float NewDisplacementY = NodePositions[i].Z + DisplacementVector(2 * (i - FixedNodesPassed) + 1);
			MeshData.Positions[i].Y = NewDisplacementX;
			MeshData.Positions[i].Z = NewDisplacementY;
		}
	}

	RealtimeMesh->RemoveSection(SectionInGroup);
	RealtimeMesh->RemoveSectionGroup(GroupKey);

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		true);
}

//This function will need to move to base class but it will need auxilliary functions to build a force vector dynamically
VectorXf ARMCTestActor::ConstructForceVector(TArray<int32> ForcedNodesIn, TArray<int32> FixedNodesIn, float ForceX, float ForceY, int NodeCount)
{
	int ActiveNodeCount = NodeCount - FixedNodes.Num();

	int FixedNodesPassed = 0;

	VectorXf forceVector(ActiveNodeCount * 2);

	for (int i = 0; i < NodeCount; i++)
	{
		if (FixedNodesIn.Contains(i))
		{
			FixedNodesPassed += 1;
		}
		else if (ForcedNodesIn.Contains(i))
		{
			forceVector( 2 * (i - FixedNodesPassed) ) = ForceX;
			forceVector( 2 * (i - FixedNodesPassed) + 1) = ForceY;
		}
		else
		{
			forceVector(2 * (i - FixedNodesPassed)) = 0.0f;
			forceVector(2 * (i - FixedNodesPassed) + 1) = 0.0f;
		}
	}

	return forceVector;
}

// This one probably should not move to base class except on a temporary basis: We need a more dynamic way of setting force vectors from game interactions
TArray<int32> ARMCTestActor::SetContigousForcedNodes(int stop, int start)
{
	TArray<int32> forcedNodes;

	for (int i = start; i < stop; i++)
	{
		forcedNodes.Add(i);
	}

	return forcedNodes;
}

// This one is not needed in base class
void ARMCTestActor::UpdateTriangle(float DeltaTime)
{
	float velocity = 10;

	MeshData.Positions[0].Z += velocity * DeltaTime;
	MeshData.Positions[1].Z += velocity * DeltaTime;
	MeshData.Positions[2].Z += velocity * DeltaTime;

	RealtimeMesh->RemoveSection(SectionInGroup);
	RealtimeMesh->RemoveSectionGroup(GroupKey);

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		true);

}

//All of the functions below here will be needed in base class pretty much as is. We will hopefully set fixed nodes in-game via UI at some point but that's not
//anywhere near ready yet, plus the Kinverse mesh is produced already knowing the fixed nodes, so that's not an option until the full design and meshing process
//is implemented in Unreal, possibly still outsourced to python
void ARMCTestActor::ReadInNodes(FString NodesFilePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Path = NodesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	if (CSVLines.Num() == 0) return;

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		FString aString = CSVLines[i];
		TArray<FString> StringArray = {};
		aString.ParseIntoArray(StringArray, TEXT(","), true);
		
		NodePositions.Add(FVector(FCString::Atof(*StringArray[0]), FCString::Atof(*StringArray[1]), FCString::Atof(*StringArray[2])));
	}

}

void ARMCTestActor::ReadInTriangles(FString TrianglesFilePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Path = TrianglesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	if (CSVLines.Num() == 0) return;

	for (int i = 0; i < CSVLines.Num() - 1; i++)
	{
		TriangleIndices.Add(FCString::Atoi(*CSVLines[i]));
	}
}

MatrixXf ARMCTestActor::ReadInGlobalMatrixInverse(FString MatrixInverseFilePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Path = MatrixInverseFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	MatrixXf kInverse(CSVLines.Num() - 1, CSVLines.Num() - 1);

	UE_LOG(LogTemp, Display, TEXT("CSVLines Length: %d"), CSVLines.Num());

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

void ARMCTestActor::ReadInFixedNodes(FString FixedNodesFilePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	FString Path = FixedNodesFilePath;

	TArray<FString> CSVLines = GetCSVFile(Path);

	if (CSVLines.Num() == 0) return;
	
	FString aString = CSVLines[0];
	TArray<FString> StringArray = {};
	aString.ParseIntoArray(StringArray, TEXT(","), true);

	for (int i = 0; i < StringArray.Num(); i++)
	{
		FixedNodes.Add(FCString::Atoi(*StringArray[i]));
	}
}

TArray<FString> ARMCTestActor::GetCSVFile(FString Path)
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

