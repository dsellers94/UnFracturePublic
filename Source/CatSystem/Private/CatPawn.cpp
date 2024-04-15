// Copyright Daniel Sellers 2023 - 2024


#include "CatPawn.h"

ACatPawn::ACatPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMesh);

	CurrentCompressable;

	CatSpeed = 300;

	CatState = ECatState::ECS_Fall;

	CatDirection = ECatDirection::ECD_Right;

	TurnOnSit = true;

	NodeSearchRange = 10;

	OnActorHit.AddDynamic(this, &ACatPawn::OnHit);
	OnActorBeginOverlap.AddDynamic(this, &ACatPawn::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACatPawn::OnEndOverlap);
}

void ACatPawn::BeginPlay()
{
	Super::BeginPlay();

	USceneComponent* Component = GetRootComponent();

	RootPrimComp = Cast<UPrimitiveComponent>(Component);
}

void ACatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetCatDirection();
	CatAction();
}

void ACatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACatPawn::SetCatDirection()
{
	switch (CatDirection)
	{
	case ECatDirection::ECD_Left:
		PreferredDirection = FVector(0, -1, 0);
		break;
	case ECatDirection::ECD_Right:
		PreferredDirection = FVector(0, 1, 0);
		break;
	}
}

void ACatPawn::CatAction()
{
	switch (CatState)
	{
	case ECatState::ECS_Walk:
		CatWalk();
		break;
	case ECatState::ECS_Sit:
		if (TurnOnSit)
		{
			switch (CatDirection)
			{
			case ECatDirection::ECD_Left:
				CatDirection = ECatDirection::ECD_Right;
				break;
			case ECatDirection::ECD_Right:
				CatDirection = ECatDirection::ECD_Left;
				break;
			}
			SetCatDirection();
			CatTryContinue();
		}
		else
		{
			CatTryTransition();
		}
		break;
	case ECatState::ECS_Fall:
		RootPrimComp->SetSimulatePhysics(true);
		break;
	}
}

void ACatPawn::CatWalk()
{
	if (NextNodeIndex == -1)
	{
		CatState = ECatState::ECS_Sit;
		return;
	}

	FVector NextNodeLocationInWorldSpace = CompressableLocation + CompressableMeshDataPositions[NextNodeIndex];

	FVector CatNextNodeSeparationVector = NextNodeLocationInWorldSpace - GetActorLocation();

	float Theta = GetRelativeRotation(CatNextNodeSeparationVector, PreferredDirection);

	float DeltaSeconds = GetWorld()->GetDeltaSeconds();

	if (Theta < PI / 2)
	{
		
		if (CatNextNodeSeparationVector.Length() < CatSpeed * DeltaSeconds )
		{
			SetActorLocation(NextNodeLocationInWorldSpace, false);

			CompressableMeshDataPositions = CurrentCompressable->GetMeshDataPositions();

			NextNodeIndex = GetNextNode(CurrentIIndex, CurrentJIndex, CatDirection);
		}
		else
		{
			SetActorLocation(GetActorLocation() + CatSpeed * DeltaSeconds * CatNextNodeSeparationVector.GetSafeNormal(), false);
		}
		
	}
	else
	{
		CatState = ECatState::ECS_Sit;
	}
}

void ACatPawn::CatTryContinue()
{	
	TArray<AActor*> OverlappedActors;
	GetOverlappingActors(OverlappedActors, AActor::StaticClass());

	for (AActor* Actor : OverlappedActors)
		{
		ICompressableInterface* CandidateCompressable = Cast<ICompressableInterface>(Actor);

		if (CandidateCompressable)
		{
			SetNewCompressable(CandidateCompressable);
			NextNodeIndex = GetNextNode(CurrentIIndex, CurrentJIndex, CatDirection);

			if (NextNodeIndex != -1)
			{
				CatState = ECatState::ECS_Walk;
				return;
			}
		}
	}
}

void ACatPawn::CatTryTransition()
{
	ResetCurrentNode();
	NextNodeIndex = GetNextNode(CurrentIIndex, CurrentJIndex, CatDirection);
	SetActorLocation(GetActorLocation() + FVector(0, 0, -1), false);
	SetActorLocation(GetActorLocation() + FVector(0, 0, 1), false);
	CatState = ECatState::ECS_Walk;
}

void ACatPawn::ResetCurrentNode()
{
	CompressableMeshDataPositions = CurrentCompressable->GetMeshDataPositions();

	FVector CatLocationInCompressableSpace = GetActorLocation() - CompressableLocation;

	TArray<int32> ClosestNodeList = CurrentCompressable->GetClosestNodesSorted(CatLocationInCompressableSpace, 10);

	bool AcceptableNodeFound = false;

	for (int32 NodeIndex : ClosestNodeList)
	{
		if (CheckNodeIsCorrectDirection(NodeIndex, PreferredDirection) && CheckIsTopNode(NodeIndex))
		{
			AcceptableNodeFound = true;
			CurrentNodeIndex = NodeIndex;
			break;
		}
	}

	if (AcceptableNodeFound)
	{
		CatState = ECatState::ECS_Walk;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No acceptable nodes found at reset"), CurrentNodeIndex);
		CatState = ECatState::ECS_Sit;
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Current: %d"), CurrentNodeIndex);

	for (int i = 0; i < CompressableNodeGridMap.rows(); i++)
	{
		for (int j = 0; j < CompressableNodeGridMap.cols(); j++)
		{
			if (CompressableNodeGridMap(i, j) == CurrentNodeIndex)
			{
				CurrentIIndex = i;
				CurrentJIndex = j;
				break;
			}
		}
	}
}

int ACatPawn::GetNextNode(int iIndex, int jIndex, ECatDirection inCatDirection)
{
	int CandidateNodeIndex;

	switch (CatDirection)
	{
	case ECatDirection::ECD_Left:
		CandidateNodeIndex = NextNodeScan(-1, iIndex, jIndex);
		break;
	case ECatDirection::ECD_Right:
		CandidateNodeIndex = NextNodeScan(0, iIndex, jIndex);
		break;
	}

	return CandidateNodeIndex;
}

int ACatPawn::NextNodeScan(int StartIndex, int iIndex, int jIndex)
{
	int candidateNodeIndex;
	for (int k = -1; k < 2; k++)
	{
		for (int l = StartIndex; l < StartIndex + 2; l++)
		{
			if (k == 0 && l == 0) continue;

			candidateNodeIndex = CompressableNodeGridMap(iIndex + l, jIndex + k);

			if (CompressableEdgeNodes.Contains(candidateNodeIndex) && CheckIsTopNode(candidateNodeIndex))
			{
				CurrentNodeIndex = candidateNodeIndex;
				CurrentIIndex += l;
				CurrentJIndex += k;
				return candidateNodeIndex;
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No acceptable nodes found in local grid scan"));
	return -1;
}

bool ACatPawn::CheckNodeIsCorrectDirection(int nodeIndex, FVector preferredDirection)
{
	FVector NodeLocationInWorldSpace = CompressableLocation + CompressableMeshDataPositions[nodeIndex];

	FVector SeparationVector = NodeLocationInWorldSpace - GetActorLocation();

	float VectorDot = FVector::DotProduct(SeparationVector, preferredDirection);

	bool IsCorrectDirection = VectorDot > 0.0;

	return IsCorrectDirection;
}

bool ACatPawn::CheckIsTopNode(int nodeIndex)
{
	int LocalIIndex;
	int LocalJIndex;

	for (int i = 0; i < CompressableNodeGridMap.rows(); i++)
	{
		for (int j = 0; j < CompressableNodeGridMap.cols(); j++)
		{
			if (CompressableNodeGridMap(i, j) == nodeIndex)
			{
				LocalIIndex = i;
				LocalJIndex = j;
				break;
			}
		}
	}

	if (LocalJIndex == CompressableNodeGridMap.cols() - 1 || CompressableNodeGridMap(LocalIIndex, LocalJIndex + 1) == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

float ACatPawn::GetRelativeRotation(FVector separationVector, FVector referenceNormal)
{
	float VectorDot = FVector::DotProduct(separationVector, referenceNormal);

	float theta = UKismetMathLibrary::Acos(VectorDot);

	return theta;
}

void ACatPawn::SetNewCompressable(ICompressableInterface* NewCompressable)
{
	CurrentCompressable = NewCompressable;
	CompressableEdgeNodes = CurrentCompressable->GetEdgeNodes();
	CompressableMeshDataPositions = CurrentCompressable->GetMeshDataPositions();
	CompressableLocation = CurrentCompressable->GetCompressableLocation();
	CompressableNodeGridMap = CurrentCompressable->GetNodeGridMap();

	ResetCurrentNode();

	NextNodeIndex = GetNextNode(CurrentIIndex, CurrentJIndex, CatDirection);
}

void ACatPawn::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
	ICompressableInterface* CompressableHit = Cast<ICompressableInterface>(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("Overlap Detected"));

	if (CompressableHit)
	{
		SetNewCompressable(CompressableHit);

		RootPrimComp->SetSimulatePhysics(false);

		CatState = ECatState::ECS_Walk;
	}
}

void ACatPawn::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	ICompressableInterface* CompressableOverlapped = Cast<ICompressableInterface>(OtherActor);

	UE_LOG(LogTemp, Warning, TEXT("Overlap Detected"));

	if (CompressableOverlapped) 
	{
		SetNewCompressable(CompressableOverlapped);

		RootPrimComp->SetSimulatePhysics(false);

		CatState = ECatState::ECS_Walk;
	}
}

void ACatPawn::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Ended"));

	ICompressableInterface* CompressableOverlapped = Cast<ICompressableInterface>(OtherActor);

	if (CompressableOverlapped && CompressableOverlapped == CurrentCompressable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Compressable Contact Lost"));

		CatState = ECatState::ECS_Fall;
	}

}
