// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterState.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/States/CharacterStateMachine.h"

const FPlayerInputs& UCharacterState::GetInputs() const
{
	return Controller->GetPlayerInputs();
}

void UCharacterState::StateInit(UCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;

#if WITH_EDITOR
	if (StateMachine == nullptr)
	{
		const FString Message = FString::Printf(TEXT("StateMachine is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
		return;
	}
#endif

	Character = InStateMachine->GetCharacter();

#if WITH_EDITOR
	if (Character == nullptr)
	{
		const FString Message = FString::Printf(TEXT("Character is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
		return;
	}
#endif

	AController* TargetController = Character->GetController();
	if (TargetController == nullptr)
	{
		return;
	}

	Controller = Cast<AFirstPersonController>(TargetController);

#if WITH_EDITOR
	if (Controller == nullptr)
	{
		const FString Message = FString::Printf(TEXT("Controller is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
	}
#endif
}

void UCharacterState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	PlayStateAnimation();
}

void UCharacterState::StateExit_Implementation(const ECharacterStateID& NextStateID) {}

void UCharacterState::StateTick_Implementation(float DeltaTime)
{
	if (!bAllowCameraMovement)
	{
		return;
	}

	Character->AddControllerYawInput(GetInputs().InputLook.X);
	Character->AddControllerPitchInput(GetInputs().InputLook.Y);
}

void UCharacterState::PlayStateAnimation()
{
	if(StateAnimation == nullptr)
	{
		Character->StopAnimMontage(nullptr);
		return;
	}

	Character->PlayAnimMontage(StateAnimation);
}
