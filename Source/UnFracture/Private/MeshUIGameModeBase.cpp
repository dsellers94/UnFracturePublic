// Copyright Daniel Sellers 2023


#include "MeshUIGameModeBase.h"
#include "Kismet/GameplayStatics.h"

AMeshUIGameModeBase::AMeshUIGameModeBase()
{
	Ny = 40;
	Nz = 40;
	OrthogDelta = 1.0;

	GenerationDataFileName = "GenerationData";
}

void AMeshUIGameModeBase::AddToCornerNodePositions(FVector Position)
{
	CornerNodePositions.Add(Position);
	UE_LOG(LogTemp, Warning, TEXT("Added Position: %s"), *Position.ToString());
}

void AMeshUIGameModeBase::ConfirmCornerPositions()
{
	UE_LOG(LogTemp, Display, TEXT("Corner Positions Confirmed"));
	if (SpawnedMeshGenerator && CornerNodePositions.Num() > 2)
	{
		TArray<float> meshSpacingData = CalculateMeshSpacingData(CornerNodePositions, Ny, Nz);
		DeltaY = meshSpacingData[0];
		DeltaZ = meshSpacingData[1];
		CornerNodePositions = AutoCorrectEdgeLines(CornerNodePositions, DeltaY, DeltaZ);
		SpawnedMeshGenerator->SetNy(Ny);
		SpawnedMeshGenerator->SetNz(Nz);
		SpawnedMeshGenerator->GeneratePhaseOne(CornerNodePositions);
		ClearAllNodePositionMarkersOfSelectedClass(CornerNodeMarkerClass);
		PlaceCorrectedCornerNodePositionsMarkers();
		PlaceEdgeNodePositionMarkers();
		UE_LOG(LogTemp, Display, TEXT("DeltaY: %f"), DeltaY);
		UE_LOG(LogTemp, Display, TEXT("DeltaZ: %f"), DeltaZ);

	}
}

TArray<float> AMeshUIGameModeBase::CalculateMeshSpacingData(TArray<FVector> inputCornerNodePositions, int inputNy, int inputNz)
{
	TArray<float> meshSpacingData;

	YMin = 1000000.0;
	float YMax = -1000000.0;
	ZMin = 1000000.0;
	float ZMax = -1000000.0;

	for (int i = 0; i < inputCornerNodePositions.Num(); i++)
	{
		float YPos = inputCornerNodePositions[i].Y;
		float ZPos = inputCornerNodePositions[i].Z;
		if (YPos > YMax) YMax = YPos;
		if (YPos < YMin) YMin = YPos;
		if (ZPos > ZMax) ZMax = ZPos;
		if (ZPos < ZMin) ZMin = ZPos;
	}

	DeltaY = (YMax - YMin) / inputNy;
	DeltaZ = (ZMax - ZMin) / inputNz;

	meshSpacingData.Add(DeltaY);
	meshSpacingData.Add(DeltaZ);
	return meshSpacingData;
}

TArray<FVector> AMeshUIGameModeBase::AutoCorrectEdgeLines(TArray<FVector> inputCornerNodePositions, float inputDeltaY, float inputDeltaZ)
{
	TArray<FVector> CorrectedCornerNodePositions;
	CorrectedCornerNodePositions.Add(FVector(0.0, inputCornerNodePositions[0].Y - YMin, inputCornerNodePositions[0].Z - ZMin));
	for (int i = 1; i < inputCornerNodePositions.Num(); i++)
	{
		FVector NewPosition;
		if (abs(inputCornerNodePositions[i].Y - inputCornerNodePositions[i - 1].Y) < OrthogDelta * DeltaY)
		{
			NewPosition.Y = inputCornerNodePositions[i - 1].Y;
		}
		else NewPosition.Y = inputCornerNodePositions[i].Y;

		if (abs(inputCornerNodePositions[i].Z - inputCornerNodePositions[i - 1].Z) < OrthogDelta * DeltaZ)
		{
			NewPosition.Z = inputCornerNodePositions[i - 1].Z;
		}
		else NewPosition.Z = inputCornerNodePositions[i].Z;

		if (i == inputCornerNodePositions.Num() - 1)
		{
			if (abs(inputCornerNodePositions[i].Y - inputCornerNodePositions[0].Y) < OrthogDelta * DeltaY)
			{
				NewPosition.Y = inputCornerNodePositions[0].Y;
			}
			else NewPosition.Y = inputCornerNodePositions[i].Y;

			if (abs(inputCornerNodePositions[i].Z - inputCornerNodePositions[0].Z) < OrthogDelta * DeltaZ)
			{
				NewPosition.Z = inputCornerNodePositions[0].Z;
			}
			else NewPosition.Z = inputCornerNodePositions[i].Z;
		}

		//Correct Offset so that smallest x and y values sit on zero
		NewPosition.Y -= YMin;
		NewPosition.Z -= ZMin;
		CorrectedCornerNodePositions.Add(NewPosition);
	
	}
	
	return CorrectedCornerNodePositions;
}

void AMeshUIGameModeBase::ClearAllNodePositionMarkersOfSelectedClass(TSubclassOf<ANodePositionMarker> NodeMarkerClassToDestroy)
{
	TArray<AActor*> NodePositionMarkers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), NodeMarkerClassToDestroy, NodePositionMarkers);
	for (AActor* Marker : NodePositionMarkers)
	{
		Marker->Destroy();
	}
}

void AMeshUIGameModeBase::PlaceCorrectedCornerNodePositionsMarkers()
{
	TArray<AActor*> OriginPositionMarker;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), OriginMarkerClass, OriginPositionMarker);
	FVector OriginPositionInWorldSpace;
	if (OriginPositionMarker.Num() != 0)
	{
		OriginPositionInWorldSpace = OriginPositionMarker[0]->GetActorLocation();
	}
	for (FVector CornerNodePosition : CornerNodePositions)
	{
		FActorSpawnParameters SpawnInfo;
		FVector Location(CornerNodePosition.Z + OriginPositionInWorldSpace.X, CornerNodePosition.Y + OriginPositionInWorldSpace.Y, 0.0);
		FRotator Rotation(0.0, 0.0, 0.0);
		GetWorld()->SpawnActor<ANodePositionMarker>(CornerNodeMarkerClass, Location, Rotation, SpawnInfo);
	}
}

void AMeshUIGameModeBase::PlaceEdgeNodePositionMarkers()
{
	TArray<AActor*> OriginPositionMarker;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), OriginMarkerClass, OriginPositionMarker);
	FVector OriginPositionInWorldSpace;
	if (OriginPositionMarker.Num() != 0)
	{
		OriginPositionInWorldSpace = OriginPositionMarker[0]->GetActorLocation();
	}

	TArray<FVector> nodePositions = SpawnedMeshGenerator->GetNodePositions();
	TArray<int32> edgeNodeIndices = SpawnedMeshGenerator->GetEdgeNodeIndices();
	for (int i = 0; i < nodePositions.Num(); i++)
	{
		if (edgeNodeIndices.Contains(i))
		{
			FActorSpawnParameters SpawnInfo;
			FVector Location(nodePositions[i].Z + OriginPositionInWorldSpace.X, nodePositions[i].Y + OriginPositionInWorldSpace.Y, 0.0);
			FRotator Rotation(0.0, 0.0, 0.0);
			GetWorld()->SpawnActor<ANodePositionMarker>(EdgeNodeMarkerClass, Location, Rotation, SpawnInfo);
		}
		
	}
}

void AMeshUIGameModeBase::PlaceInternalNodePositionMarkers()
{
	TArray<AActor*> OriginPositionMarker;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), OriginMarkerClass, OriginPositionMarker);
	FVector OriginPositionInWorldSpace;
	if (OriginPositionMarker.Num() != 0)
	{
		OriginPositionInWorldSpace = OriginPositionMarker[0]->GetActorLocation();
	}
	TArray<FVector> NodePositions = SpawnedMeshGenerator->GetNodePositions();
	for (int i = 0; i < NodePositions.Num(); i++)
	{
		FActorSpawnParameters SpawnInfo;
		FVector Location(NodePositions[i].Z + OriginPositionInWorldSpace.X, NodePositions[i].Y + OriginPositionInWorldSpace.Y, 0.0);
		FRotator Rotation(0.0, 0.0, 0.0);
		GetWorld()->SpawnActor<ANodePositionMarker>(InternalNodeMarkerClass, Location, Rotation, SpawnInfo);
	}
}

void AMeshUIGameModeBase::IngestMeshDensityInput(int inputNy, int inputNz)
{
	Ny = inputNy;
	Nz = inputNz;

	MeshDensity.Add(0);
	MeshDensity.Add(Ny);
	MeshDensity.Add(Nz);

	UE_LOG(LogTemp, Display, TEXT("Ingested Density Values: %d, %d"), inputNy, inputNz);
}

void AMeshUIGameModeBase::IngestAndPassFixedNodes(FVector Start, FVector End)
{
	SpawnedMeshGenerator->SetFixedNodesInBox(Start, End);

	FixedNodeBoxes.Add(Start);
	FixedNodeBoxes.Add(End);

}

void AMeshUIGameModeBase::IngestAndPassMeshName(FString Name)
{
	SpawnedMeshGenerator->SetMeshName(Name);
	MeshName = Name;
}

void AMeshUIGameModeBase::IngestAndPassElasticMod(float elasticMod)
{
	SpawnedMeshGenerator->SetElasticMod(elasticMod);

	ElasticMod = elasticMod;

}

void AMeshUIGameModeBase::IngestAndPassFocalPoint(FVector FocalPointPosition)
{
	SpawnedMeshGenerator->AddFocalPoint(FocalPointPosition);
	FocalPointPositions.Add(FocalPointPosition);
}

void AMeshUIGameModeBase::FinishMeshGeneration()
{
	SpawnedMeshGenerator->GeneratePhaseTwo();
}

void AMeshUIGameModeBase::SaveMesh()
{
	SpawnedMeshGenerator->WriteAllToCSV();

	TSharedPtr<FJsonValue> NewJsonDataObject = MeshLoadUtility::CreateMeshDataArrayElement(
		MeshName, 
		CornerNodePositions, 
		FixedNodeBoxes, 
		MeshDensity, 
		ElasticMod,
		FocalPointPositions);
	MeshLoadUtility::AppendJsonObjectToArray(NewJsonDataObject, GenerationDataFileName);
}


void AMeshUIGameModeBase::ResetAll()
{
	SpawnedMeshGenerator->ResetMeshGenerator();
	ClearAllNodePositionMarkersOfSelectedClass(CornerNodeMarkerClass);
	ClearAllNodePositionMarkersOfSelectedClass(InternalNodeMarkerClass);
	ClearAllNodePositionMarkersOfSelectedClass(EdgeNodeMarkerClass);
	CornerNodePositions.Empty();
	FixedNodeBoxes.Empty();
	MeshDensity.Empty();
	FocalPointPositions.Empty();
}