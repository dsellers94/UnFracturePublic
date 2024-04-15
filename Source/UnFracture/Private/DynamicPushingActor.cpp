// Copyright Daniel Sellers 2023


#include "DynamicPushingActor.h"
#include "CompressableGameActor.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Math/Quat.h"

// Sets default values
ADynamicPushingActor::ADynamicPushingActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AssignedComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));

	SetRootComponent(AssignedComponent);

	RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(false);
	RealtimeMeshComponent->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	RealtimeMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RealtimeMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	RealtimeMeshComponent->SetupAttachment(RootComponent);

	OnActorHit.AddDynamic(this, &ADynamicPushingActor::OnHit);

	Width = 120.0;

	PushingVector = FVector(0.0, 0.0, -50.0);

	PushingMode = false;

	Velocity = FVector(0.0, 50.0, 0.0);

	IsMoving = true;

	UseAltMesh = true;

	IsSolid = true;

}

// Called when the game starts or when spawned
void ADynamicPushingActor::BeginPlay()
{
	Super::BeginPlay();
	
	RealtimeMesh = NewObject<URealtimeMeshSimple>();
	RealtimeMeshComponent->SetRealtimeMesh(RealtimeMesh);
	RealtimeMeshComponent->SetWorldScale3D(FVector(1.0, 1.0, 1.0));

}

// Called every frame
void ADynamicPushingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateLocation(Velocity, IsMoving, DeltaTime);

	if (PushingMode)
	{
		EffectedCompressableMeshDataPositions = EffectedCompressable->GetMeshDataPositions();
		if (MeshDataPositionsHasChanged())
		{
			if (UseAltMesh)
			{
				UpdateMeshAlt();
			}
			else
			{
				UpdateMesh();
			}
		}
	}
}

void ADynamicPushingActor::InitializeMesh()
{
	//NOTE this works differently than the node position indexing in compressable base. This class is aware of but not laoding in all node positions which exist on that
	// the effected compressable mesh, so the index only refers to the sorted target nodes plus the center point index
	TArray<int32> TargetNodesSortedByRotation = GetTargetNodesSortedByRotation();

	for (int32 TargetNodeIndex : TargetNodesSortedByRotation)
	{
		NodePositions.Add(ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[TargetNodeIndex]));
	}
	NodePositions.Add(FVector(0.0, 0.0, 0.0));
	int CenterPointIndex = NodePositions.Num() - 1;
	for (int i = 0; i < TargetNodesSortedByRotation.Num() - 1; i++)
	{
		TriangleIndices.Add(i);
		TriangleIndices.Add(i + 1);
		TriangleIndices.Add(CenterPointIndex);
	}

	TArray<FVector> NormalsBuffer;
	TArray<FVector> TangentsBuffer;
	TArray<FVector2D> UV0Buffer;

	float YMin = 1000000;
	float ZMin = 1000000;
	float YMax = -1000000;
	float ZMax = -1000000;

	for (int i = 0; i < NodePositions.Num(); i++)
	{
		NormalsBuffer.Add(FVector(-1, 0, 0));
		TangentsBuffer.Add(FVector(0, 1, 0));
		if (NodePositions[i].Y < YMin) YMin = NodePositions[i].Y;
		if (NodePositions[i].Y > YMax) YMax = NodePositions[i].Y;
		if (NodePositions[i].Z < ZMin) ZMin = NodePositions[i].Z;
		if (NodePositions[i].Z > ZMax) ZMax = NodePositions[i].Z;
	}

	float HorizontalRange = YMax - YMin;
	float VerticalRange = ZMax - ZMin;

	for (int i = 0; i < NodePositions.Num(); i++)
	{
		UV0Buffer.Add(FVector2D(NodePositions[i].Y / (HorizontalRange), NodePositions[i].Z / (VerticalRange)));
	}

	MeshData.Triangles = TriangleIndices;
	MeshData.Positions = NodePositions;
	MeshData.Normals = NormalsBuffer;
	MeshData.UV0 = UV0Buffer;
	MeshData.Tangents = TangentsBuffer;

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		IsSolid);
}

void ADynamicPushingActor::UpdateMesh()
{

	TArray<int32> TargetNodesSortedByRotation = GetTargetNodesSortedByRotation();

	for (int i = 0; i < TargetNodesSortedByRotation.Num(); i++)
	{
		int TargetNodeIndex = TargetNodesSortedByRotation[i];
		MeshData.Positions[i] = ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[TargetNodeIndex]);
	}

	RealtimeMesh->RemoveSection(SectionInGroup);
	RealtimeMesh->RemoveSectionGroup(GroupKey);

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		IsSolid);

}

void ADynamicPushingActor::InitializeMeshAlt()
{
	TArray<int32> TargetNodesSortedByRotation = GetTargetNodesSortedByRotation();
	
	int NumTargetNodes = TargetNodesSortedByRotation.Num();

	NodePositions.Add(ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[0]));
	NodePositions.Add(ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[TargetNodesSortedByRotation[NumTargetNodes - 1]]));
	NodePositions.Add(FVector(0.0, 0.0, 0.0));

	int CenterPointIndex = 2;
	TriangleIndices.Add(0);
	TriangleIndices.Add(1);
	TriangleIndices.Add(CenterPointIndex);

	TArray<FVector> NormalsBuffer;
	TArray<FVector> TangentsBuffer;
	TArray<FVector2D> UV0Buffer;

	float YMin = 1000000;
	float ZMin = 1000000;
	float YMax = -1000000;
	float ZMax = -1000000;

	for (int i = 0; i < NodePositions.Num(); i++)
	{
		NormalsBuffer.Add(FVector(-1, 0, 0));
		TangentsBuffer.Add(FVector(0, 1, 0));
		if (NodePositions[i].Y < YMin) YMin = NodePositions[i].Y;
		if (NodePositions[i].Y > YMax) YMax = NodePositions[i].Y;
		if (NodePositions[i].Z < ZMin) ZMin = NodePositions[i].Z;
		if (NodePositions[i].Z > ZMax) ZMax = NodePositions[i].Z;
	}

	float HorizontalRange = YMax - YMin;
	float VerticalRange = ZMax - ZMin;

	for (int i = 0; i < NodePositions.Num(); i++)
	{
		UV0Buffer.Add(FVector2D(NodePositions[i].Y / (HorizontalRange), NodePositions[i].Z / (VerticalRange)));
	}

	MeshData.Triangles = TriangleIndices;
	MeshData.Positions = NodePositions;
	MeshData.Normals = NormalsBuffer;
	MeshData.UV0 = UV0Buffer;
	MeshData.Tangents = TangentsBuffer;

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		IsSolid);
}

void ADynamicPushingActor::UpdateMeshAlt()
{
	TArray<int32> TargetNodesSortedByRotation = GetTargetNodesSortedByRotation();

	int NumTargetNodes = TargetNodesSortedByRotation.Num();

	int TargetNodeIndex = TargetNodesSortedByRotation[0];
	MeshData.Positions[0] = ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[TargetNodeIndex]);
	TargetNodeIndex = TargetNodesSortedByRotation[NumTargetNodes - 1];
	MeshData.Positions[1] = ConvertCompressableSpaceVectorToLocal(EffectedCompressableMeshDataPositions[TargetNodeIndex]);

	RealtimeMesh->RemoveSection(SectionInGroup);
	RealtimeMesh->RemoveSectionGroup(GroupKey);

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		IsSolid);
}

TArray<int32> ADynamicPushingActor::GetTargetNodesSortedByRotation()
{
	EffectedCompressableMeshDataPositions = EffectedCompressable->GetMeshDataPositions();

	// Get and draw the average separation vector between target nodes and pushing actor every frame
	float AverageY = 0;
	float AverageZ = 0;

	for (int TargetNodeIndex : EffectedCompressableTargetNodes)
	{
		FVector SeparationVector = EffectedCompressableMeshDataPositions[TargetNodeIndex] - RestingLocationInCompressableSpace;
		AverageY += SeparationVector.Y;
		AverageZ += SeparationVector.Z;
	}
	AverageY = AverageY / EffectedCompressableTargetNodes.Num();
	AverageZ = AverageZ / EffectedCompressableTargetNodes.Num();
	FVector AverageCpNodeSeparationVector = FVector(0.0, AverageY, AverageZ);
	FRotator AverageCpNodeRotation = UKismetMathLibrary::MakeRotFromZ(AverageCpNodeSeparationVector);

	TMap<int32, float> indexRotationMap;

	for (int TargetNodeIndex : EffectedCompressableTargetNodes)
	{
		FVector SeparationVector = EffectedCompressableMeshDataPositions[TargetNodeIndex] - RestingLocationInCompressableSpace;
		FQuat QuatRotation = FQuat::FindBetweenVectors(SeparationVector, AverageCpNodeSeparationVector);
		indexRotationMap.Add(TargetNodeIndex, QuatRotation.X);
		//UE_LOG(LogTemp, Display, TEXT("Calculated Quat: %s"), *QuatRotation.ToString());
	}

	indexRotationMap.ValueSort([](float A, float B) {
		return A > B;
		});

	TArray<int32> targetNodesSortedByRotation;

	for (auto& pair : indexRotationMap)
	{
		targetNodesSortedByRotation.Add(pair.Key);
	}

	return targetNodesSortedByRotation;
}

FVector ADynamicPushingActor::ConvertCompressableSpaceVectorToLocal(FVector CompressableSpaceVector)
{
	FVector LocalVector = CompressableSpaceVector - RestingLocationInCompressableSpace;
	
	return LocalVector;
}

void ADynamicPushingActor::UpdateLocation(FVector velocity, bool isMoving, float DeltaTime)
{
	if (isMoving)
	{
		SetActorLocation(GetActorLocation() + velocity * DeltaTime, true);
	}
}

void ADynamicPushingActor::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	ACompressableGameActor* CompressableHit = Cast<ACompressableGameActor>(OtherActor);
	if (CompressableHit)
	{
		EffectedCompressable = CompressableHit;
		if (!PushingMode)
		{
			EngagePushing(EffectedCompressable);
		}
	}
}

void ADynamicPushingActor::EngagePushing(ACompressableGameActor* effectedCompressable)
{
	EngagePushingEvent();

	PushingMode = true;

	RestingLocationInCompressableSpace = EffectedCompressable->ConvertWorldVectorToLocal(GetActorLocation());
	EffectedCompressableDeltaY = EffectedCompressable->GetDeltaY();
	EffectedCompressableDeltaZ = EffectedCompressable->GetDeltaZ();

	IsMoving = false;

	EffectedCompressableTargetNodes = EffectedCompressable->GetClosestNodesToAppliedForce(RestingLocationInCompressableSpace, Width / EffectedCompressableDeltaZ);

	effectedCompressable->RegisterPushingActor(GetActorNameOrLabel(), this);

	if (UseAltMesh)
	{
		InitializeMeshAlt();
	}
	else
	{
		InitializeMesh();
	}
}

void ADynamicPushingActor::DisengagePushing(ACompressableGameActor* effectedCompressable)
{
	//PushingMode = false;
	if (effectedCompressable)
	{
		effectedCompressable->UnRegisterPushingActor(GetActorNameOrLabel());
	}
}

void ADynamicPushingActor::Disengage()
{
	DisengagePushing(EffectedCompressable);
	Destroy();
}

bool ADynamicPushingActor::MeshDataPositionsHasChanged()
{
	bool HasChanged = false;

	if (EffectedCompressableMeshDataPositions.Num() != PreviousEffectedCompressableMeshDataPositions.Num())
	{
		PreviousEffectedCompressableMeshDataPositions = EffectedCompressableMeshDataPositions;
		return true;
	}

	for (int i = 0; i < EffectedCompressableMeshDataPositions.Num(); i++)
	{
		if (EffectedCompressableMeshDataPositions[i] != PreviousEffectedCompressableMeshDataPositions[i])
		{
			HasChanged = true;
			break;
		}
	}

	PreviousEffectedCompressableMeshDataPositions = EffectedCompressableMeshDataPositions;

	return HasChanged;
}

bool ADynamicPushingActor::GetIsPushing()
{
	return PushingMode;
}

float ADynamicPushingActor::GetWidth()
{
	return Width;
}

FVector ADynamicPushingActor::GetPushingVector()
{
	return PushingVector;
}

void ADynamicPushingActor::SetPushingVector(FVector NewPushingVector)
{
	PushingVector = NewPushingVector;
}

TArray<int32> ADynamicPushingActor::GetPushingActorTargetNodes()
{
	return EffectedCompressableTargetNodes;
}

FVector ADynamicPushingActor::GetActorVelocity()
{
	return Velocity;
}

void ADynamicPushingActor::SetVelocity(FVector NewVelocity)
{
	Velocity = NewVelocity;
}

void ADynamicPushingActor::SetWidth(float width)
{
	Width = width;
}

void ADynamicPushingActor::SetIsSolid(bool inIsSolid)
{
	IsSolid = inIsSolid;
}