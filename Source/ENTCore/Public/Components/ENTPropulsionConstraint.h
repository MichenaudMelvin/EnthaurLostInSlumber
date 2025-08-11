// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ENTPropulsionConstraint.generated.h"

class AENTDefaultCharacter;
class AENTDefaultPlayerController;
class AENTNerve;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropulsionStateChanged, bool, bCanBePropelled);

UCLASS(ClassGroup = (Physics), meta = (BlueprintSpawnableComponent))
class ENTCORE_API UENTPropulsionConstraint : public UActorComponent
{
	GENERATED_BODY()

public:
	UENTPropulsionConstraint();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter);

	void ReleasePlayer(const bool DetachFromPlayer = false);

	bool IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const;

	UPROPERTY(BlueprintAssignable, Category = "Physics")
	FOnPropulsionStateChanged OnPropulsionStateChanged;

	UPROPERTY()
	TObjectPtr<AENTNerve> LinkedNerve;

	UPROPERTY()
	TObjectPtr<AENTDefaultCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<AENTDefaultPlayerController> PlayerController;

	FVector PreviousVelocity = FVector::ZeroVector;

	bool bIsAlreadyPropelled = false;

	bool bHasReleasedInteraction = false;

private:
	float DefaultMaxSpeed = 0.0f;

	bool bIsPropulsionActive = false;
};
