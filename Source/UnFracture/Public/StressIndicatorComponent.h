// Copyright Daniel Sellers 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RealtimeMeshComponent.h"
#include "RealtimeMeshSimple.h"
#include "RealtimeMeshActor.h"
#include "CompressableBase.h"
#include "StressIndicatorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UNFRACTURE_API UStressIndicatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStressIndicatorComponent();

	//Moving these two properties to public to try and get a dynamic material instance working on them
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	URealtimeMeshComponent* RealtimeMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float DeformationScale;

	UPROPERTY(EditAnywhere)
	float WarningLimit;

	UPROPERTY(EditAnywhere)
	float BreakLimit;
	
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaterialEvent();

	UFUNCTION(BlueprintCallable)
	float GetDeformationScale();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	ACompressableBase* CompressableOwner;

	URealtimeMeshSimple* RealtimeMesh;

	FRealtimeMeshSimpleMeshData MeshData;

	FRealtimeMeshSectionKey StaticSectionKey;

	FRealtimeMeshSectionGroupKey GroupKey;

	FRealtimeMeshSectionKey SectionInGroup;

	TArray<FVector> NodePositions;

	TArray<int32> TriangleIndices;

	TArray<int32> OwnerTriangleIndices;

	UPROPERTY()
	TArray<FVector> UndeformedNodePositions;

	TArray<FVector> OwnerMeshDataPositions;

	FVector OwnerLocation;

	TArray<FVector> NormalsBuffer;

	TArray<FVector2D> UV0Buffer;

	void InitializeMesh();

	UFUNCTION()
	void DrawStressWarningMesh();

	UFUNCTION()
	void InitializeVariables();

	UFUNCTION()
	void OnMeshChanged();
		
};
