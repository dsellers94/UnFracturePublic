// Copyright Daniel Sellers 2023


#include "SupportBeamPushingActor.h"
#include "CompressableGameActor.h"


ASupportBeamPushingActor::ASupportBeamPushingActor()
{
	//RealtimeMeshComponent = CreateDefaultSubobject<URealtimeMeshComponent>(TEXT("RealtimeMeshComponent"));
	//RealtimeMeshComponent->SetMobility(EComponentMobility::Movable);
	RealtimeMeshComponent->SetGenerateOverlapEvents(true);

	//RealtimeMeshComponent->SetupAttachment(RootComponent);

	IsMoving = false;

	BeamWidth = 20;
}

void ASupportBeamPushingActor::BeginPlay()
{
	Super::BeginPlay();

	//InitializeMesh();
}

void ASupportBeamPushingActor::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

	UpdateMesh();

	ConstituentsAreMoving = false;

	for (ADynamicPushingActor* Pusher : ConstituentActors)
	{
		if (Pusher->IsMoving) ConstituentsAreMoving = true;
	}

	for (ADynamicPushingActor* Pusher : ConstituentActors)
	{
		if (ConstituentsAreMoving)
		{
			Pusher->PushingMode = false;
		}
		else
		{
			Pusher->PushingMode = true;
		}
	}

}


void ASupportBeamPushingActor::RegisterConstituentActor(ADynamicPushingActor* NewActor)
{
	ConstituentActors.Add(NewActor);
}

void ASupportBeamPushingActor::Disengage()
{
	for (ADynamicPushingActor* Pusher : ConstituentActors)
	{
		Pusher->DisengagePushing(Pusher->EffectedCompressable);
		Pusher->Destroy();
	}
	Destroy();
}


void ASupportBeamPushingActor::InitializeMesh()
{
	CalculateBeamMesh();

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		false);
}

void ASupportBeamPushingActor::UpdateMesh()
{
	FVector FirstPusherLocation = ConstituentActors[0]->GetActorLocation();
	FVector SecondPusherLocation = ConstituentActors[1]->GetActorLocation();

	FVector PusherSeparationVector = SecondPusherLocation - FirstPusherLocation;

	FRotator PerpRotation = FRotator(0.0, 0.0, -90.0);

	FVector PerpSeparationVector = PerpRotation.RotateVector(PusherSeparationVector);

	FVector PerpSeparationNormal = PerpSeparationVector.GetSafeNormal();

	MeshData.Positions[0] = ConvertWorldVectorToLocal(FirstPusherLocation - PerpSeparationNormal * BeamWidth / 2);
	MeshData.Positions[1] = ConvertWorldVectorToLocal(FirstPusherLocation + PerpSeparationNormal * BeamWidth / 2);
	MeshData.Positions[2] = ConvertWorldVectorToLocal(SecondPusherLocation + PerpSeparationNormal * BeamWidth / 2);
	MeshData.Positions[3] = ConvertWorldVectorToLocal(SecondPusherLocation - PerpSeparationNormal * BeamWidth / 2);

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

	MeshData.UV0 = UV0Buffer;

	RealtimeMesh->RemoveSection(SectionInGroup);
	RealtimeMesh->RemoveSectionGroup(GroupKey);

	GroupKey = RealtimeMesh->CreateSectionGroupWithMesh(0, MeshData);

	SectionInGroup = RealtimeMesh->CreateSectionInGroup(
		GroupKey,
		FRealtimeMeshSectionConfig(ERealtimeMeshSectionDrawType::Static, 0),
		FRealtimeMeshStreamRange(0, MeshData.Positions.Num(), 0, MeshData.Triangles.Num()),
		true);
}

FVector ASupportBeamPushingActor::ConvertWorldVectorToLocal(FVector WorldSpaceVector)
{
	FVector LocalSpaceVector = WorldSpaceVector - GetActorLocation();

	return LocalSpaceVector;
}

void ASupportBeamPushingActor::CalculateBeamMesh()
{
	FVector FirstPusherLocation = ConstituentActors[0]->GetActorLocation();
	FVector SecondPusherLocation = ConstituentActors[1]->GetActorLocation();

	FVector PusherSeparationVector = SecondPusherLocation - FirstPusherLocation;

	FRotator PerpRotation = FRotator(0.0, 0.0, -90.0);

	FVector PerpSeparationVector = PerpRotation.RotateVector(PusherSeparationVector);

	FVector PerpSeparationNormal = PerpSeparationVector.GetSafeNormal();

	NodePositions.Add(ConvertWorldVectorToLocal(FirstPusherLocation - PerpSeparationNormal * BeamWidth / 2));
	NodePositions.Add(ConvertWorldVectorToLocal(FirstPusherLocation + PerpSeparationNormal * BeamWidth / 2));
	NodePositions.Add(ConvertWorldVectorToLocal(SecondPusherLocation + PerpSeparationNormal * BeamWidth / 2));
	NodePositions.Add(ConvertWorldVectorToLocal(SecondPusherLocation - PerpSeparationNormal * BeamWidth / 2));

	TriangleIndices.Add(2);
	TriangleIndices.Add(1);
	TriangleIndices.Add(0);
	TriangleIndices.Add(0);
	TriangleIndices.Add(3);
	TriangleIndices.Add(2);

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
}