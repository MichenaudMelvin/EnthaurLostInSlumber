// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ParasitePawn.generated.h"

class UGravityPawnMovement;
class AAIPath;
class UBoxComponent;
class UAIPerceptionComponent;

UCLASS()
class DEPTHAIMODULE_API AParasitePawn : public APawn
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

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AAIPath> TargetPath;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PathKeyName = "AIPath";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName WalkOnFloorKeyName = "WalkOnFloor";

public:
	UBoxComponent* GetCollisionComp() {return ParasiteCollision;}
};
