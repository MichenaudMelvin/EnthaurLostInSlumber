// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ParasitePawn.generated.h"

class UBoxComponent;
class UAIPerceptionComponent;
class UFloatingPawnMovement;

UCLASS()
class DEPTHAIMODULE_API AParasitePawn : public APawn
{
	GENERATED_BODY()

public:
	AParasitePawn();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> Box;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UFloatingPawnMovement> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ParasiteMesh;
};
