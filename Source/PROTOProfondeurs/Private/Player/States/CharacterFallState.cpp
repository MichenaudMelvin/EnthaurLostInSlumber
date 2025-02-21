// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterFallState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

UCharacterFallState::UCharacterFallState()
{
	StateID = ECharacterStateID::Fall;
}

void UCharacterFallState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	Super::StateEnter_Implementation(PreviousStateID);

	if (PreviousStateID != ECharacterStateID::Jump)
	{
		bCanDoCoyoteTime = true;
		CoyoteTime = 0.0f;
	}
	else
	{
		bCanDoCoyoteTime = false;
	}

	Character->GetCharacterMovement()->GravityScale = GravityScale;
	Character->GetCharacterMovement()->AirControl = AirControl;
}

void UCharacterFallState::StateTick_Implementation(float DeltaTime)
{
	Super::StateTick_Implementation(DeltaTime);

	if(bCanDoCoyoteTime)
	{
		CoyoteTime += DeltaTime;
		if(CoyoteTime >= CoyoteTimeMaxDuration)
		{
			bCanDoCoyoteTime = false;

#if WITH_EDITORONLY_DATA
			if(bDebugState)
			{
				FVector CapsuleLocation = Character->GetCapsuleComponent()->GetComponentLocation();
				FRotator CapsuleRotation = Character->GetCapsuleComponent()->GetComponentRotation();
				float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
				float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
				UKismetSystemLibrary::DrawDebugCapsule(Character, CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FLinearColor::Red, 5.0f);
			}
#endif
		}

		if (GetInputs().bInputJump)
		{
			StateMachine->ChangeState(ECharacterStateID::Jump);
		}
	}

	if(Character->GetCharacterMovement()->IsFalling())
	{
		return;
	}

	StateMachine->ChangeState(ECharacterStateID::Idle);
}
