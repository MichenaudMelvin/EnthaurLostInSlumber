// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerToNervePhysicConstraint.generated.h"

class ANerve;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROTOPROFONDEURS_API UPlayerToNervePhysicConstraint : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerToNervePhysicConstraint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	void Init(ANerve* vLinkedNerve, ACharacter* vPlayerCharacter);
	void ReleasePlayer();

	UPROPERTY()
	ANerve* LinkedNerve;
	UPROPERTY()
	ACharacter* PlayerCharacter;
	UPROPERTY()
	FVector PreviousVelocity;

private:
	UPROPERTY()
	float DefaultMaxSpeed;
};
