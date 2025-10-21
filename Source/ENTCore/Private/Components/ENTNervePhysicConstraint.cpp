// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ENTNervePhysicConstraint.h"

#include "GameElements/ENTNerve.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/States/ENTCharacterStateMachine.h"


void UENTNervePhysicConstraint::UpdateDefaultMaxSpeed(UENTCharacterState* State, const EENTCharacterStateID& StateID)
{
	Super::UpdateDefaultMaxSpeed(State, StateID);

	if (!bShouldChildUpdateMaxSpeed) return;

	PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed *= LinkedNerve->GetSlowDownFactor();
}

void UENTNervePhysicConstraint::Init(AENTNerve* vLinkedNerve, ACharacter* vPlayerCharacter)
{
	Super::Init(vLinkedNerve, vPlayerCharacter);

	if (!bShouldChildInit) return;

	UpdateDefaultMaxSpeed(PlayerStateMachine->GetCurrentState(), PlayerStateMachine->GetCurrentStateID());
	PlayerStateMachine->OnChangeState.AddDynamic(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed);
}

void UENTNervePhysicConstraint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (PlayerStateMachine->OnChangeState.IsAlreadyBound(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed))
	{
		PlayerStateMachine->OnChangeState.RemoveDynamic(this, &UENTNervePhysicConstraint::UpdateDefaultMaxSpeed);
	}
}

void UENTNervePhysicConstraint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShouldChildTick) return;

	const float CurrentSpeed = PlayerCharacter->GetVelocity().Size() / PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed;

	CameraBobbingTime += DeltaTime * (BaseFrequency);

	const float RollOffset = FMath::Sin(CameraBobbingTime * PI * 2.0f) * BaseAmplitude * CurrentSpeed;

	AENTDefaultPlayerController* PC = PlayerCharacter->GetPlayerController();
	if (PC)
	{
		FRotator ControlRot = PC->GetControlRotation();
		FRotator TargetRotation = ControlRot;
		TargetRotation.Roll = RollOffset;
		constexpr float InterpSpeed = 5.0f;
		const FRotator SmoothRotation = FMath::RInterpTo(ControlRot, TargetRotation, DeltaTime, InterpSpeed);
		PC->SetControlRotation(SmoothRotation);
	}

	if (PlayerController->GetPlayerInputs().bInputInteractPressed)
	{
		ReleasePlayer(true);
	}
}
