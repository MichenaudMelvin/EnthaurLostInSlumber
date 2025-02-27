// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Player/States/CharacterStateMachine.h"
#include "Player/States/CharacterStateSettings.h"

const FPlayerInputs& UCharacterState::GetInputs() const
{
	return Controller->GetPlayerInputs();
}

bool UCharacterState::IsFalling() const
{
	return Character->GetCharacterMovement()->IsFalling();
}

void UCharacterState::CheckGround()
{
	FHitResult HitResult;
	if (!Character->GroundTrace(HitResult))
	{
		return;
	}

	if (HitResult.PhysMaterial == nullptr)
	{
		return;
	}

	if (HitResult.PhysMaterial->SurfaceType != CurrentSurface)
	{
		CurrentSurface = HitResult.PhysMaterial->SurfaceType;
		OnWalkOnNewSurface(CurrentSurface);
	}
}

void UCharacterState::OnWalkOnNewSurface_Implementation(const TEnumAsByte<EPhysicalSurface>& NewSurface)
{
	const UCharacterStateSettings* StateSettings = GetDefault<UCharacterStateSettings>();

	if (NewSurface == StateSettings->SlipperySurface)
	{
		StateMachine->ChangeState(ECharacterStateID::Slide);
	}
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
	if(ViewBobbing == nullptr)
	{
		return;
	}

	Controller->ClientStartCameraShake(ViewBobbing, 1.0f, ECameraShakePlaySpace::World);
}

void UCharacterState::StateTick_Implementation(float DeltaTime)
{
	if (bCheckGround)
	{
		CheckGround();
	}

	if (bAllowCameraMovement)
	{
		Character->AddControllerYawInput(GetInputs().InputLook.X);
		Character->AddControllerPitchInput(GetInputs().InputLook.Y);
	}
}

void UCharacterState::StateExit_Implementation(const ECharacterStateID& NextStateID)
{
	if(ViewBobbing == nullptr)
	{
		return;
	}

	Controller->ClientStopCameraShake(ViewBobbing);
}
