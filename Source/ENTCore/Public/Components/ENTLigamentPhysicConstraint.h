// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTPhysicConstraint.h"
#include "ENTLigamentPhysicConstraint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPropulsionStateChanged, bool, bCanBePropelled);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ENTCORE_API UENTLigamentPhysicConstraint : public UENTPhysicConstraint
{
	GENERATED_BODY()

public:
	
	virtual void Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void ReleasePlayer(const bool DetachFromPlayer = false) override;
		
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool IsMovingTowardsPosition(const FVector& TargetPosition, float AcceptanceThreshold) const;

	UPROPERTY(BlueprintAssignable, Category = "Physics")
	FOnPropulsionStateChanged OnPropulsionStateChanged;

private:
	bool bIsPropulsionActive = false;
	
	bool bIsAlreadyPropelled = false;
};
