// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Player/States/CharacterStateMachine.h"
#include "Physics/TracePhysicsSettings.h"
#include "Saves/SettingsSave.h"
#include "Saves/SettingsSubsystem.h"

void UCharacterState::StartCameraShake()
{
	if(!ViewBobbing)
	{
		return;
	}

	if (GetSettings() && !GetSettings()->bViewBobbing)
	{
		return;
	}

	CurrentViewBobbing = Cast<UViewBobbing>(Controller->PlayerCameraManager->StartCameraShake(ViewBobbing, 1.0f, ECameraShakePlaySpace::World));
}

void UCharacterState::StopCameraShake()
{
	if(!ViewBobbing)
	{
		return;
	}

	Controller->ClientStopCameraShake(ViewBobbing);
}

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

USettingsSave* UCharacterState::GetSettings() const
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(Character);
	if (!GameInstance)
	{
		return nullptr;
	}

	USettingsSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<USettingsSubsystem>();
	if (!SettingsSubsystem)
	{
		return nullptr;
	}

	return SettingsSubsystem->GetSettings();
}

void UCharacterState::OnWalkOnNewSurface_Implementation(const TEnumAsByte<EPhysicalSurface>& NewSurface)
{
	const UTracePhysicsSettings* StateSettings = GetDefault<UTracePhysicsSettings>();

	if (NewSurface == StateSettings->SlipperySurface)
	{
		StateMachine->ChangeState(ECharacterStateID::Slide);
	}
}

void UCharacterState::StateInit(UCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;

#if WITH_EDITOR
	if (!StateMachine)
	{
		const FString Message = FString::Printf(TEXT("StateMachine is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
		return;
	}
#endif

	Character = InStateMachine->GetCharacter();

#if WITH_EDITOR
	if (!Character)
	{
		const FString Message = FString::Printf(TEXT("Character is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
		return;
	}
#endif

	AController* TargetController = Character->GetController();
	if (!TargetController)
	{
		return;
	}

	Controller = Cast<AFirstPersonController>(TargetController);

#if WITH_EDITOR
	if (!Controller)
	{
		const FString Message = FString::Printf(TEXT("Controller is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
	}
#endif
}

void UCharacterState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID)
{
	StartCameraShake();
}

void UCharacterState::StateTick_Implementation(float DeltaTime)
{
	if (bCheckGround)
	{
		CheckGround();
	}

	if (bAllowCameraMovement)
	{
		const USettingsSave* Settings = GetSettings();
		if (!Settings)
		{
			return;
		}

		float TargetYaw = GetInputs().InputLook.X * Settings->MouseSensitivity;
		float TargetPitch = GetInputs().InputLook.Y * Settings->MouseSensitivity * (Settings->bInvertYAxis ? -1 : 1);

		Character->AddControllerYawInput(TargetYaw);
		Character->AddControllerPitchInput(TargetPitch);
	}
}

void UCharacterState::StateExit_Implementation(const ECharacterStateID& NextStateID)
{
	StopCameraShake();
}
