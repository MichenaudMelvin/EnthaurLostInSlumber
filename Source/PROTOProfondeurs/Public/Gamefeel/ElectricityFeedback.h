// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricityFeedback.generated.h"

UCLASS()
class PROTOPROFONDEURS_API AElectricityFeedback : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AElectricityFeedback();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UPostProcessComponent> Electricity;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius;
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> Material;
};
