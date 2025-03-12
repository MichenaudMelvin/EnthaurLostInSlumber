// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterState.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Player/States/CharacterStateMachine.h"
#include "Physics/TracePhysicsSettings.h"
#include "Saves/SettingsSave.h"
#include "Saves/SettingsSubsystem.h"

#pragma region States

UCharacterState::UCharacterState()
{
	ViewBobbing = UViewBobbing::GetEmptyOscillator();
}

void UCharacterState::StateInit(UCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
	TargetSteering = MaxSteering;

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

void UCharacterState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID){}

void UCharacterState::StateTick_Implementation(float DeltaTime)
{
	if (bCheckGround)
	{
		CheckGround();
	}

	CameraMovement(DeltaTime);
	UpdateCameraFOV(DeltaTime);
	UpdateViewBobbing(DeltaTime);
	UpdateCameraSteering(DeltaTime);
}

void UCharacterState::StateExit_Implementation(const ECharacterStateID& NextStateID){}

#pragma endregion

#pragma region Character

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
	const UTracePhysicsSettings* StateSettings = GetDefault<UTracePhysicsSettings>();

	if (NewSurface == StateSettings->SlipperySurface)
	{
		StateMachine->ChangeState(ECharacterStateID::Slide);
	}
}

#pragma endregion

#pragma region Camera

void UCharacterState::CameraMovement(float DeltaTime)
{
	if (!bAllowCameraMovement)
	{
		return;
	}

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

void UCharacterState::UpdateCameraFOV(float DeltaTime)
{
	float CurrentFOV = Character->GetCamera()->FieldOfView;
	float FOV = FMath::Lerp(CurrentFOV, TargetFOV, DeltaTime);
	Character->GetCamera()->SetFieldOfView(FOV);
}

void UCharacterState::UpdateViewBobbing(float DeltaTime)
{
	if (!Character->GetViewBobbingObject() || !GetSettings())
	{
		return;
	}

	const FWaveOscillator CurrentWaveOscillator = Character->GetViewBobbingObject()->GetOscillator();
	const FWaveOscillator TargetWaveOscillator = GetSettings()->bViewBobbing ? ViewBobbing : UViewBobbing::GetEmptyOscillator();

	float TargetAmplitude = FMath::Lerp(CurrentWaveOscillator.Amplitude, TargetWaveOscillator.Amplitude, DeltaTime);
	float TargetFrequency = FMath::Lerp(CurrentWaveOscillator.Frequency, TargetWaveOscillator.Frequency, DeltaTime);

	FWaveOscillator Oscillator;
	Oscillator.Amplitude = TargetAmplitude;
	Oscillator.Frequency = TargetFrequency;
	Oscillator.InitialOffsetType = ViewBobbing.InitialOffsetType;

	Character->GetViewBobbingObject()->SetOscillator(Oscillator);
}

void UCharacterState::UpdateCameraSteering(float DeltaTime)
{
	FRotator TargetRotation = Controller->GetControlRotation();

	if (!GetSettings() || !GetSettings()->bCameraSteering)
	{
		TargetRotation.Roll = 0;
		Controller->SetControlRotation(TargetRotation);
		return;
	}

	float CurrentRoll = TargetRotation.Roll;
	while (CurrentRoll > 180.0f)
	{
		CurrentRoll -= 360.0f;
	}

	while(CurrentRoll < -180.0f)
	{
		CurrentRoll += 360.0f;
	}

	float LerpRoll = FMath::Lerp(CurrentRoll, TargetSteering, DeltaTime * SteeringSpeed);
	TargetRotation.Roll = LerpRoll;
	Controller->SetControlRotation(TargetRotation);
}

#pragma endregion

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
