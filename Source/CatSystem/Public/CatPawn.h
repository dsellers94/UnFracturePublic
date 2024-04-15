// Copyright Daniel Sellers 2023 - 2024

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Compressable/Public/CompressableInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include <Eigen/Dense>
#include "CatPawn.generated.h"

using Eigen::MatrixXf;

UENUM(BlueprintType)
enum class ECatState : uint8
{
	ECS_Walk	UMETA(DisplayName = "Walk"),
	ECS_Sit		UMETA(DisplayName = "Sit"),
	ECS_Jump	UMETA(DisplayName = "Jump"),
	ECS_Fall	UMETA(DisplayName = "Fall")
};

UENUM(BlueprintType)
enum class ECatDirection : uint8
{
	ECD_Left	UMETA(DisplayName = "Left"),
	ECD_Right	UMETA(DisplayName = "Right")
};

UCLASS()
class CATSYSTEM_API ACatPawn : public APawn
{
	GENERATED_BODY()

public:
	ACatPawn();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMesh;

	int NextNodeIndex;
	
	int CurrentNodeIndex;

	int CurrentIIndex;

	int CurrentJIndex;

	FVector PreferredDirection;

	UPrimitiveComponent* RootPrimComp;

	ICompressableInterface* CurrentCompressable;

	TArray<int32> CompressableEdgeNodes;

	TArray<FVector> CompressableMeshDataPositions;

	FVector CompressableLocation;

	MatrixXf CompressableNodeGridMap;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECatState CatState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECatDirection CatDirection;

	UPROPERTY(EditAnywhere)
	float CatSpeed;

	UPROPERTY(EditAnywhere)
	bool TurnOnSit;

	UPROPERTY(EditAnywhere)
	int NodeSearchRange;

	void SetCatDirection();

	void SetNewCompressable(ICompressableInterface* NewCompressable);

	UFUNCTION()
	void CatAction();

	UFUNCTION()
	void CatWalk();

	UFUNCTION()
	void CatTryContinue();

	UFUNCTION()
	void CatTryTransition();

	UFUNCTION()
	void ResetCurrentNode();

	UFUNCTION()
	int GetNextNode(int iIndex, int jIndex, ECatDirection inCatDirection);

	UFUNCTION()
	int NextNodeScan(int StartIndex, int iIndex, int jIndex);

	UFUNCTION()
	bool CheckNodeIsCorrectDirection(int NodeIndex, FVector preferredDirection);

	UFUNCTION()
	bool CheckIsTopNode(int NodeIndex);

	UFUNCTION()
	float GetRelativeRotation(FVector separationVector, FVector referenceNormal);

	UFUNCTION()
	void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);
};
