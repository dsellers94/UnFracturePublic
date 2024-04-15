// Copyright Daniel Sellers 2023


#include "StressIndicatorComponent.h"
#include "CompressableBase.h"

UStressIndicatorComponent::UStressIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	AActor* Owner = GetOwner();
	CompressableOwner = Cast<ACompressableBase>(Owner);
	
	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	
	if (CompressableOwner)
	{
		CompressableOwner->OnInitComplete.AddDynamic(this, &UStressIndicatorComponent::InitializeVariables);
		CompressableOwner->OnMeshChanged.AddDynamic(this, &UStressIndicatorComponent::OnMeshChanged);
	}

	WarningLimit = 50;
	BreakLimit = 100;

	DeformationScale = 0.0;

	//UndeformedNodePositions.SetNum(10000);
}


void UStressIndicatorComponent::BeginPlay()
{
	Super::BeginPlay();

	RealtimeMesh = NewObject<URealtimeMeshSimple>();
	RealtimeMeshComponent->SetRealtimeMesh(RealtimeMesh);

	//if (CompressableOwner)
	//{
	//	CompressableOwner->OnInitComplete.AddDynamic(this, &UStressIndicatorComponent::InitializeVariables);
	//	CompressableOwner->OnMeshChanged.AddDynamic(this, &UStressIndicatorComponent::OnMeshChanged);
	//}
}


void UStressIndicatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UStressIndicatorComponent::OnMeshChanged()
{
	DrawStressWarningMesh();
	UpdateMaterialEvent();
}

void UStressIndicatorComponent::InitializeVariables()
{
	if (CompressableOwner->MeshIsValid)
	{
		UndeformedNodePositions = CompressableOwner->GetNodePositions();
		OwnerLocation = CompressableOwner->GetActorLocation() + FVector(-1.0, 0.0, 0.0);
		InitializeMesh();
	}
}

void UStressIndicatorComponent::InitializeMesh()
{
	NodePositions.Add(OwnerLocation);
	NodePositions.Add(OwnerLocation + FVector(0.0, 100, 0.0));
	NodePositions.Add(OwnerLocation + FVector(0.0, 0.0, 100));
	TriangleIndices.Add(0);
	TriangleIndices.Add(1);
	TriangleIndices.Add(2);
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	NormalsBuffer.Add(FVector(-1, 0, 0));
	TArray<FVector> TangentsBuffer;
	TArray<FLinearColor> vertexColorsBuffer;
	UV0Buffer.Add(FVector2D(0, 0));
	UV0Buffer.Add(FVector2D(1, 0));
	UV0Buffer.Add(FVector2D(0, 1));
	TArray<FColor> ColorsBuffer;
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
}

void UStressIndicatorComponent::DrawStressWarningMesh()
{
	if (!CompressableOwner) return;

	OwnerLocation = CompressableOwner->GetActorLocation() + FVector(-1.0, 0.0, 0.0);

	if (UndeformedNodePositions.Num() == 0) UndeformedNodePositions = CompressableOwner->GetNodePositions();

	OwnerMeshDataPositions = CompressableOwner->GetMeshDataPositions();

	OwnerTriangleIndices = CompressableOwner->GetTriangleIndices();

	NodePositions.Empty();
	TriangleIndices.Empty();
	NormalsBuffer.Empty();
	UV0Buffer.Empty();
	int NodesAdded = 0;

	float LargestDeformation = 0.0;
	DeformationScale = 0.0;
	for (int i = 0; i < OwnerTriangleIndices.Num(); i += 3)
	{
		int TriangleIndex = OwnerTriangleIndices[i];
		float Deformation = FVector::Dist(OwnerMeshDataPositions[TriangleIndex], UndeformedNodePositions[TriangleIndex]);
		if (Deformation > LargestDeformation) LargestDeformation = Deformation;
		if (Deformation > WarningLimit)
		{
			DeformationScale = FMath::Clamp((LargestDeformation - WarningLimit) / (BreakLimit - WarningLimit), 0, 1);
			for (int j = 0; j < 3; j++)
			{
				NodePositions.Add(OwnerLocation + OwnerMeshDataPositions[OwnerTriangleIndices[i + j]]);
				TriangleIndices.Add(NodesAdded);
				NodesAdded += 1;
				NormalsBuffer.Add(FVector(-1.0, 0.0, 0.0));
				UV0Buffer.Add(FVector2D(0.0, 0.0));
			}
		}
	}

	MeshData.Triangles = TriangleIndices;
	MeshData.Positions = NodePositions;
	MeshData.Normals = NormalsBuffer;
	MeshData.UV0 = UV0Buffer;

	const auto LOD = RealtimeMesh->GetMesh()->GetLOD(SectionInGroup.GetLODKey());
	if (LOD)
	{
		if (const auto SectionGroup = LOD->GetSectionGroup(SectionInGroup.GetSectionGroupKey()))
		{
			RealtimeMesh->RemoveSection(SectionInGroup);
			RealtimeMesh->RemoveSectionGroup(GroupKey);
		}
	}

	//RealtimeMesh->RemoveSection(SectionInGroup);
	//RealtimeMesh->RemoveSectionGroup(GroupKey);

	if (NodePositions.Num() != 0)
	{
		GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

		SectionInGroup = RealtimeMesh->CreateSectionInGroup(
			GroupKey,
			FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
			FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
			true);
	}
}

float UStressIndicatorComponent::GetDeformationScale()
{
	return DeformationScale;
}