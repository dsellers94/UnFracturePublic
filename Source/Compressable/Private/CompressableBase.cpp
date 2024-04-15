// Copyright Daniel Sellers 2023


#include "CompressableBase.h"
#include "Misc/FileHelper.h"
#include "Containers/UnrealString.h"
#include <iostream>
#include <fstream>
#include <string>
#include "GenericPlatform/GenericPlatformMath.h"
#include "UnFractureGameModeBase.h"

using namespace std;
using Eigen::MatrixXd;
using Eigen::MatrixXf;
using Eigen::VectorXf;
using Eigen::VectorXd;

// Class Core Functions
ACompressableBase::ACompressableBase()
{
	PrimaryActorTick.bCanEverTick = true;

	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(true);
	RealtimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);

	RealtimeMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RealtimeMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	SetRootComponent(RealtimeMeshComponent);

	MeshIsValid = true;

	StillInterpolating = false;
}

void ACompressableBase::BeginPlay()
{
	Super::BeginPlay();
	
	RealtimeMesh = NewObject<URealtimeMeshSimple>();
	RealtimeMeshComponent->SetRealtimeMesh(RealtimeMesh);

	InitializeMesh();
}

void ACompressableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// RMC and Eigen Management Functions

void ACompressableBase::InitializeMesh()
{
	MeshIsValid = MeshLoadUtility::CheckValidMeshFile("Files/" + MeshName + "_positions.csv");

	if (MeshIsValid)
	{
		AUnFractureGameModeBase* UnFractureGameMode = Cast<AUnFractureGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

		if (UnFractureGameMode && UnFractureGameMode->MeshNameList.Contains(MeshName))
		{
			CompressableData = UnFractureGameMode->GetCompressableDataByMeshName(MeshName);
		}
		else
		{
			CompressableData = MeshLoadUtility::GetCompressableDataByMeshName(MeshName);
		}

		TArray<FVector> NormalsBuffer;
		TArray<FVector> TangentsBuffer;
		TArray<FLinearColor> vertexColorsBuffer;
		TArray<FVector2D> UV0Buffer;
		TArray<FColor> ColorsBuffer;

		for (int i = 0; i < CompressableData.NodePositions.Num(); i++)
		{
			NormalsBuffer.Add(FVector(-1, 0, 0));
			TangentsBuffer.Add(FVector(0, 1, 0));
		}

		for (int i = 0; i < CompressableData.NodePositions.Num(); i++)
		{
			//UV0Buffer.Add(FVector2D(
				//CompressableData.NodePositions[i].Y / (CompressableData.Ny * CompressableData.DeltaY), 
				//CompressableData.NodePositions[i].Z / (CompressableData.Nz * CompressableData.DeltaZ)));
			
			UV0Buffer.Add(FVector2D(CompressableData.NodePositions[i].Y / 100, CompressableData.NodePositions[i].Z / 100));
		}

		MeshData.Triangles = CompressableData.TriangleIndices;
		MeshData.Positions = CompressableData.NodePositions;
		MeshData.UV0 = UV0Buffer;
		MeshData.Normals = NormalsBuffer;
		MeshData.Tangents = TangentsBuffer;

		GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

		SectionInGroup = RealtimeMesh->CreateSectionInGroup(
			GroupKey,
			FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
			FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
			true);

	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("INVALID MESH!"));
	}

	OnInitComplete.Broadcast();
}

void ACompressableBase::UpdateMeshPositions()
{
	if (MeshIsValid)
	{
		for (int32 Node : CompressableData.EdgeNodes)
		{
			if (CompressableData.FixedNodes.Contains(Node))
			{
				continue;
			}
			else
			{
				float NewPositionY = CompressableData.NodePositions[Node].Y + DisplacementVector(2 * CompressableData.NodeMatrixMap[Node]);
				float NewPositionZ = CompressableData.NodePositions[Node].Z + DisplacementVector(2 * CompressableData.NodeMatrixMap[Node] + 1);
				MeshData.Positions[Node].Y = NewPositionY;
				MeshData.Positions[Node].Z = NewPositionZ;
			}
		}
	}
}

// This is a dynamic version of UpdateMeshPositions function which support interpolation (ie. gradual movement to calculated deformed position)
// It should be called on tick in an implementation class, similar to how UpdateMeshPositions() is called in CompressableGameActor.
// The variable bool StillInterpolating exists entirely to support this function and is meant as a flag to carry out updates to the mesh
// even if the calculated displacement vector has not changed since the last tick. 
void ACompressableBase::UpdateMeshPositionsDynamic(float plasticity)
{
	if (MeshIsValid)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		
		StillInterpolating = false;

		for (int32 Node : CompressableData.EdgeNodes)
		{
			if (CompressableData.FixedNodes.Contains(Node))
			{
				continue;
			}
			else
			{
				float TargetPositionY = CompressableData.NodePositions[Node].Y + DisplacementVector(2 * CompressableData.NodeMatrixMap[Node]);
				float TargetPositionZ = CompressableData.NodePositions[Node].Z + DisplacementVector(2 * CompressableData.NodeMatrixMap[Node] + 1);
				
				float SeparationY = TargetPositionY - MeshData.Positions[Node].Y;
				float SeparationZ = TargetPositionZ - MeshData.Positions[Node].Z;
				float SeparationMag = FMath::Sqrt(FMath::Pow(SeparationY, 2) + FMath::Pow(SeparationZ, 2));

				if (plasticity * DeltaTime >= SeparationMag)
				{
					MeshData.Positions[Node].Y = TargetPositionY;
					MeshData.Positions[Node].Z = TargetPositionZ;
				}
				else
				{
					StillInterpolating = true;

					float NewPositionY = MeshData.Positions[Node].Y + plasticity * DeltaTime * SeparationY / SeparationMag;
					float NewPositionZ = MeshData.Positions[Node].Z + plasticity * DeltaTime * SeparationZ / SeparationMag;

					MeshData.Positions[Node].Y = NewPositionY;
					MeshData.Positions[Node].Z = NewPositionZ;
				}

			
			}
		}
	}
}

void ACompressableBase::UpdateFocalPointPositions()
{
	FVector AverageDisplacementVector;

	for (int32 Node : CompressableData.EdgeNodes)
	{
		AverageDisplacementVector += MeshData.Positions[Node] - CompressableData.NodePositions[Node];
	}

	AverageDisplacementVector /= CompressableData.EdgeNodes.Num();

	for (int i = 0; i < CompressableData.FocalPointCount; i++)
	{
		FVector UndeformedFocalPointPosition = CompressableData.NodePositions.Last(i);
		FVector NewFocalPointPosition = UndeformedFocalPointPosition + AverageDisplacementVector;
		MeshData.Positions.Last(i) = NewFocalPointPosition;
	}
}

void ACompressableBase::UpdateMesh()
{
	if (MeshIsValid)
	{
		RealtimeMesh->RemoveSection(SectionInGroup);
		RealtimeMesh->RemoveSectionGroup(GroupKey);

		GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

		SectionInGroup = RealtimeMesh->CreateSectionInGroup(
			GroupKey,
			FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
			FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
			true);
	}
}

VectorXf ACompressableBase::ConstructForceVector(
	TArray<int32> ForcedNodesIn,
	TArray<int32> FixedNodesIn,
	TMap<int32, float> forcedNodeYMagnitudes,
	TMap<int32, float> forcedNodeZMagnitudes)
{
	VectorXf forceVector(CompressableData.EdgeNodes.Num() * 2);

	for (int32 Node : CompressableData.EdgeNodes)
	{
		if (FixedNodesIn.Contains(Node))
		{
			continue; 
		}
		else if (ForcedNodesIn.Contains(Node))
		{
			forceVector(2 * CompressableData.NodeMatrixMap[Node]) = forcedNodeYMagnitudes[Node];
			forceVector(2 * CompressableData.NodeMatrixMap[Node] + 1) = forcedNodeZMagnitudes[Node];
		}
		else
		{
			forceVector(2 * CompressableData.NodeMatrixMap[Node]) = 0.0f;
			forceVector(2 * CompressableData.NodeMatrixMap[Node] + 1) = 0.0f;
		}
	}

	return forceVector;
}

VectorXf ACompressableBase::CalculateDisplacementVector()
{
	VectorXf displacementVector(CompressableData.KInverse.rows());

	displacementVector = CompressableData.KInverse * ForceVector;

	return displacementVector;
}

bool ACompressableBase::DisplacementVectorHasChanged()
{
	bool HasChanged = false;

	if (DisplacementVector.rows() != PreviousDisplacementVector.rows())
	{
		PreviousDisplacementVector = DisplacementVector;
		return true;
	}

	for (int i = 0; i < DisplacementVector.rows(); i++)
	{
		if (DisplacementVector(i) != PreviousDisplacementVector(i))
		{
			HasChanged = true;
			break;
		}
	}
	
	PreviousDisplacementVector = DisplacementVector;

	return HasChanged;
}

// Object Interaction Handling Functions

int32 ACompressableBase::GetClosestSingleNode(FVector ForceLocation)
{
	float MinDistance = INT_MAX;
	int32 ClosestNode;

	for (int EdgeNodeIndex : CompressableData.EdgeNodes)
	{
		float NodeForceDistance = FVector::Dist(MeshData.Positions[EdgeNodeIndex], ForceLocation); 

		if (NodeForceDistance < MinDistance)
		{
			MinDistance = NodeForceDistance;
			ClosestNode = EdgeNodeIndex;
		}
	}

	return ClosestNode;
}

TArray<int32> ACompressableBase::GetClosestNodesSorted(FVector TargetPosition, int NodeCount)
{
	float MinDistance = INT_MAX;

	TMap<int32, float> ClosestNodeMap;

	for (int EdgeNodeIndex : CompressableData.EdgeNodes) //Loop through all edgenodes to find the closest ones
	{
		FVector SeparationVector = MeshData.Positions[EdgeNodeIndex] + GetActorLocation() - TargetPosition;

		if (ClosestNodeMap.Num() < NodeCount) // If we haven't yet reached the buffer size limit, just add any nodes (that are in the right direction)
		{
			ClosestNodeMap.Add(EdgeNodeIndex, SeparationVector.Length());
		}
		else // Once we've reached the buffer size limit, identify the farthest node in the list and remove it if the next node is closer
		{
			float NodeTargetDistance = FVector::Dist(MeshData.Positions[EdgeNodeIndex], TargetPosition);
			float MaxDistanceInList = 0.0f;
			int FireableNodeIndex;
			for (auto& pair : ClosestNodeMap)
			{
				float FireableNodeDistance = FVector::Dist(MeshData.Positions[pair.Key], TargetPosition);
				if (FireableNodeDistance > MaxDistanceInList)
				{
					MaxDistanceInList = FireableNodeDistance;
					FireableNodeIndex = pair.Key;
				}
			}
			if (NodeTargetDistance < MaxDistanceInList) // replace the "fireable node" if the new one is closer
			{
				ClosestNodeMap.Remove(FireableNodeIndex);
				ClosestNodeMap.Add(EdgeNodeIndex, NodeTargetDistance);
			}
		}
	}

	ClosestNodeMap.ValueSort([](float A, float B) {
		return A < B;
		});

	TArray<int32> closestNodesSorted;

	for (auto& pair : ClosestNodeMap) // Construct the new, sorted node index buffer for the cat to use
	{
		closestNodesSorted.Add(pair.Key);
	}

	return closestNodesSorted;
}

int32 ACompressableBase::GetClosestSingleUndeformedNodeToAppliedForce(FVector ForceLocation)
{
	float MinDistance = INT_MAX;
	int32 ClosestNode;

	for (int EdgeNodeIndex : CompressableData.EdgeNodes)
	{
		float NodeForceDistance = FVector::Dist(CompressableData.NodePositions[EdgeNodeIndex], ForceLocation);

		if (NodeForceDistance < MinDistance)
		{
			MinDistance = NodeForceDistance;
			ClosestNode = EdgeNodeIndex;
		}
	}

	return ClosestNode;
}

TArray<int32> ACompressableBase::GetClosestNodesToAppliedForce(FVector ForceLocation, int NodalWidth)
{
	float MinDistance = INT_MAX;
	TArray<int32> CloseNodeList;
	TArray<int32> EmptyList;
	TArray<int32> ClosestNodeList;

	//This could probably be optimized some more by noting the MaxDistanceInList after each node and only checking subsequent nodes if they're potentially closer
	for (int EdgeNodeIndex : CompressableData.EdgeNodes)
	{
		if (ClosestNodeList.Num() < NodalWidth)
		{
			ClosestNodeList.Add(EdgeNodeIndex);
		}
		else
		{
			float NodeForceDistance = FVector::Dist(MeshData.Positions[EdgeNodeIndex], ForceLocation);
			float MaxDistanceInList = 0.0f;
			int FireableNode;
			for (int i = 0; i < ClosestNodeList.Num(); i++)
			{
				float FireableNodeDistance = FVector::Dist(MeshData.Positions[ClosestNodeList[i]], ForceLocation);
				if (FireableNodeDistance > MaxDistanceInList)
				{
					MaxDistanceInList = FireableNodeDistance;
					FireableNode = ClosestNodeList[i];
				}
			}
			if (NodeForceDistance < MaxDistanceInList)
			{
				ClosestNodeList.Remove(FireableNode);
				ClosestNodeList.Add(EdgeNodeIndex);
			}
		}
	}
	return ClosestNodeList;
}

TArray<int32> ACompressableBase::GetClosestUndeformedNodesToAppliedForce(FVector ForceLocation, int NodalWidth)
{
	float MinDistance = INT_MAX;
	TArray<int32> CloseNodeList;
	TArray<int32> EmptyList;
	TArray<int32> ClosestNodeList;

	//This could probably be optimized some more by noting the MaxDistanceInList after each node and only checking subsequent nodes if they're potentially closer
	for (int EdgeNodeIndex : CompressableData.EdgeNodes)
	{
		if (ClosestNodeList.Num() < NodalWidth)
		{
			ClosestNodeList.Add(EdgeNodeIndex);
		}
		else
		{
			float NodeForceDistance = FVector::Dist(CompressableData.NodePositions[EdgeNodeIndex], ForceLocation);
			float MaxDistanceInList = 0.0f;
			int FireableNode;
			for (int i = 0; i < ClosestNodeList.Num(); i++)
			{
				float FireableNodeDistance = FVector::Dist(CompressableData.NodePositions[ClosestNodeList[i]], ForceLocation);
				if (FireableNodeDistance > MaxDistanceInList)
				{
					MaxDistanceInList = FireableNodeDistance;
					FireableNode = ClosestNodeList[i];
				}
			}
			if (NodeForceDistance < MaxDistanceInList)
			{
				ClosestNodeList.Remove(FireableNode);
				ClosestNodeList.Add(EdgeNodeIndex);
			}
		}
	}
	return ClosestNodeList;
}

FVector ACompressableBase::ConvertWorldVectorToLocal(FVector WorldVector)
{
	FVector LocalVector = WorldVector - GetActorLocation();

	return LocalVector;
}

//Interface Utilities

TArray<int32> ACompressableBase::GetEdgeNodes()
{
	return CompressableData.EdgeNodes;
}

TArray<FVector> ACompressableBase::GetNodePositions()
{
	return CompressableData.NodePositions;
}

TArray<FVector> ACompressableBase::GetMeshDataPositions()
{
	return MeshData.Positions;
}

FVector ACompressableBase::GetCompressableLocation()
{
	return GetActorLocation();
}

MatrixXf ACompressableBase::GetNodeGridMap()
{
	return CompressableData.NodeGridMap;
}

float ACompressableBase::GetDeltaY()
{
	return CompressableData.DeltaY;
}

float ACompressableBase::GetDeltaZ()
{
	return CompressableData.DeltaZ;
}

FString ACompressableBase::GetMeshName()
{
	return MeshName;
}

TArray<int32> ACompressableBase::GetTriangleIndices()
{
	return CompressableData.TriangleIndices;
}