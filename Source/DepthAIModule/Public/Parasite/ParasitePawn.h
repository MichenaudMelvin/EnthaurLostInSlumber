// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveAIData.h"
#include "GameFramework/Pawn.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "ParasitePawn.generated.h"

class AParasiteController;

USTRUCT(BlueprintType)
struct FParaSiteData : public FAIData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	int PathIndex = -1;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	int PathDirection = 0;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	bool bWalkOnFloor = false;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	FVector MoveToLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "AI|Blackboard")
	bool bHeardNoise = false;
};

class UGravityPawnMovement;
class AAIPath;
class UBoxComponent;
class UAIPerceptionComponent;

UCLASS()
class DEPTHAIMODULE_API AParasitePawn : public APawn, public ISaveGameElementInterface
{
	GENERATED_BODY()

public:
	AParasitePawn();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ParasiteCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGravityPawnMovement> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ParasiteMesh;

	UPROPERTY(BlueprintReadOnly, Category = "AI");
	TObjectPtr<AParasiteController> ParasiteController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AAIPath> TargetPath;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PathKeyName = "AIPath";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName WalkOnFloorKeyName = "WalkOnFloor";


	virtual void PossessedBy(AController* NewController) override;

public:
	UBoxComponent* GetCollisionComp() {return ParasiteCollision;}


#pragma region Save

protected:
	bool bLoadBlackboardData = false;

	FParaSiteData BlackboardData;

public:
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;

#pragma endregion
};
