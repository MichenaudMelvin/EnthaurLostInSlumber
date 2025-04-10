// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerToNervePhysicConstraint.generated.h"

class AFirstPersonCharacter;
class AFirstPersonController;
class ANerve;

UCLASS(ClassGroup = (Physics), meta=(BlueprintSpawnableComponent))
class PROTOPROFONDEURS_API UPlayerToNervePhysicConstraint : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerToNervePhysicConstraint();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(ANerve* vLinkedNerve, ACharacter* vPlayerCharacter);
	void ReleasePlayer(const bool DetachFromPlayer = false);
	bool IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const;

	UPROPERTY()
	TObjectPtr<ANerve> LinkedNerve;

	UPROPERTY()
	TObjectPtr<AFirstPersonCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<AFirstPersonController> PlayerController;

	UPROPERTY()
	FVector PreviousVelocity;

	UPROPERTY()
	bool IsAlreadyPropulsing = false;

	bool bHasReleasedInteraction = false;

private:
	UPROPERTY()
	float DefaultMaxSpeed;
	UPROPERTY()
	bool IsPropultionActive;
};
