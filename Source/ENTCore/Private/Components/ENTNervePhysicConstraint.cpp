// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ENTNervePhysicConstraint.h"

#include "GameElements/ENTNerve.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/Camera/ENTViewBobbing.h"
#include "Player/States/ENTCharacterMoveState.h"
#include "Player/States/ENTCharacterStateMachine.h"


void UENTNervePhysicConstraint::UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID)
{
	Super::UpdateDefaultMaxSpeed(State, StateID);

	if (!bShouldChildUpdateMaxSpeed) return;

	UENTCharacterMoveState* MoveState = Cast<UENTCharacterMoveState>(State);

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed *= (MoveState)? MoveState->GetSlowDownFactor() : 1.f;
}

void UENTNervePhysicConstraint::Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	Super::Init(vLinkedNerve, vPlayerCharacter);

	if (!bShouldChildInit) return;

	UpdateDefaultMaxSpeed(PlayerStateMachine->GetCurrentState(), PlayerStateMachine->GetCurrentStateID());
	PlayerStateMachine->OnChangeState.AddDynamic(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed);

	PlayerStateMachine->LockRollBobbing(false);
}

void UENTNervePhysicConstraint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PlayerStateMachine->OnChangeState.IsAlreadyBound(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed))
	{
		PlayerStateMachine->OnChangeState.RemoveDynamic(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed);
	}
}

void UENTNervePhysicConstraint::ReleasePlayer(const bool DetachFromPlayer)
{
	PlayerStateMachine->LockRollBobbing(true);
	Super::ReleasePlayer(DetachFromPlayer);
}

void UENTNervePhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShouldChildTick) return;

	if (PlayerController->GetPlayerInputs().bInputInteractPressed)
	{
		ReleasePlayer(true);
	}
}
