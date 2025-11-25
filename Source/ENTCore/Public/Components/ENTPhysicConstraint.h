// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ENTPhysicConstraint.generated.h"

class UENTCharacterStateMachine;
class AENTDefaultCharacter;
class AENTDefaultPlayerController;
class AENTNerve;

UCLASS(Abstract, ClassGroup = (Physics), meta = (BlueprintSpawnableComponent))
class ENTCORE_API UENTPhysicConstraint : public UActorComponent
{
	GENERATED_BODY()

public:
	UENTPhysicConstraint();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter);

	virtual void ReleasePlayer(const bool DetachFromPlayer = false);

	UFUNCTION()
	virtual void UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID);

	UFUNCTION()
	AENTNerve* GetLinkedNerve(){ return LinkedNerve; }

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	float DefaultMaxSpeed = 0.0f;

	bool bShouldChildTick = true;

	bool bShouldChildInit = true;

	bool bShouldChildUpdateMaxSpeed = true;

	UPROPERTY()
	TObjectPtr<AENTNerve> LinkedNerve;

	UPROPERTY()
	TObjectPtr<AENTDefaultCharacter> PlayerCharacter;

	UPROPERTY()
	TObjectPtr<UENTCharacterStateMachine> PlayerStateMachine;

	UPROPERTY()
	TObjectPtr<AENTDefaultPlayerController> PlayerController;

private:
	
	bool bHasReleasedInteraction = false;
	
};
