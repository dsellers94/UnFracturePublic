// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshActor.h"
#include "DynamicPushingActor.generated.h"

UCLASS()
class UNFRACTURE_API ADynamicPushingActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicPushingActor();

	UFUNCTION(BlueprintCallable)
	void UpdateLocation(FVector velocity, bool isMoving, float DeltaTime);

	UPROPERTY(EditAnywhere)
	bool IsMoving;

	UPROPERTY(EditAnywhere)
	FVector Velocity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool IsSolid;

	UFUNCTION(BlueprintCallable)
	virtual void EngagePushing(ACompressableGameActor* effectedCompressable);

	UFUNCTION(BlueprintCallable)
	void DisengagePushing(ACompressableGameActor* effectedCompressable);

	UFUNCTION(BlueprintImplementableEvent)
	void EngagePushingEvent();

	UFUNCTION()
	bool GetIsPushing();

	UFUNCTION()
	float GetWidth();

	UFUNCTION()
	FVector GetPushingVector();

	UFUNCTION()
	void SetPushingVector(FVector NewPushingVector);

	UFUNCTION()
	FVector GetActorVelocity();

	UFUNCTION()
	void SetVelocity(FVector NewVelocity);

	UFUNCTION()
	void SetWidth(float width);

	UFUNCTION()
	TArray<int32> GetPushingActorTargetNodes();

	UFUNCTION()
	void SetIsSolid(bool inIsSolid);

	UPROPERTY(EditAnywhere)
	float Width;

	UPROPERTY(EditAnywhere)
	FVector PushingVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool PushingMode;

	UPROPERTY(BlueprintReadOnly)
	ACompressableGameActor* EffectedCompressable;

	UPROPERTY(EditAnywhere)
	bool UseAltMesh;

	UFUNCTION()
	void OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	virtual void InitializeMesh();

	virtual void UpdateMesh();

	void InitializeMeshAlt();

	void UpdateMeshAlt();

	TArray<int32> GetTargetNodesSortedByRotation();

	FVector ConvertCompressableSpaceVectorToLocal(FVector CompressableSpaceVector);

	virtual void Disengage();

	bool MeshDataPositionsHasChanged();

	UPROPERTY(EditAnywhere)
	USceneComponent* RootComp;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* AssignedComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	URealtimeMeshComponent* RealtimeMeshComponent;

	URealtimeMeshSimple* RealtimeMesh;

	FRealtimeMeshSimpleMeshData MeshData;

	FRealtimeMeshSectionKey StaticSectionKey;

	FRealtimeMeshSectionGroupKey GroupKey;

	FRealtimeMeshSectionKey SectionInGroup;

	TArray<FVector> NodePositions;

	TArray<int32> TriangleIndices;

	TArray<FVector> EffectedCompressableNodePositions;

	TArray<FVector> EffectedCompressableMeshDataPositions;

	TArray<FVector> PreviousEffectedCompressableMeshDataPositions;

	TArray<int32> EffectedCompressableTargetNodes;

	FVector RestingLocationInCompressableSpace;

	float EffectedCompressableDeltaY;

	float EffectedCompressableDeltaZ;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
