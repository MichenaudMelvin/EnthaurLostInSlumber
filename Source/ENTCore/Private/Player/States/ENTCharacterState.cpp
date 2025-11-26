// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/ENTCharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ENTDefaultCharacter.h"
#include "Player/ENTDefaultPlayerController.h"
#include "Player/Camera/ENTViewBobbing.h"
#include "Player/States/ENTCharacterMoveState.h"
#include "Player/States/ENTCharacterStateMachine.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

#pragma region States

#if WITH_EDITOR
FString StateToString(EENTCharacterStateID State)
{
	switch (State)
	{
		case EENTCharacterStateID::None:
			return "None";
		case EENTCharacterStateID::Idle:
			return "Idle";
		case EENTCharacterStateID::Walk:
			return "Walk";
		case EENTCharacterStateID::Sprint:
			return "Sprint";
		case EENTCharacterStateID::Crouch:
			return "Crouch";
		case EENTCharacterStateID::Jump:
			return "Jump";
		case EENTCharacterStateID::Fall:
			return "Fall";
		case EENTCharacterStateID::Interact:
			return "Interact";
		case EENTCharacterStateID::Stop:
			return "Stop";
		case EENTCharacterStateID::Anim:
			return "Anim";
	}

	return "Unknow state; Please fill the StateToString function (Player/State/ENTCharacterState.h/.cpp)";
}
#endif

UENTCharacterState::UENTCharacterState()
{
	ViewBobbing = UENTViewBobbing::GetEmptyOscillator();
}

void UENTCharacterState::StateInit(UENTCharacterStateMachine* InStateMachine)
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

	Controller = Cast<AENTDefaultPlayerController>(TargetController);

#if WITH_EDITOR
	if (!Controller)
	{
		const FString Message = FString::Printf(TEXT("Controller is null in state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
	}
#endif
}

void UENTCharacterState::StateEnter_Implementation(const EENTCharacterStateID& PreviousStateID)
{
#if WITH_EDITOR
	if (!StateMachine || !Character || !Controller)
	{
		const FString Message = FString::Printf(TEXT("Cannot start state %s (StateID is %d)"), *GetClass()->GetName(), StateID);

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));

		if (StateMachine)
		{
			StateMachine->ChangeState(EENTCharacterStateID::None);
		}
	}
#endif
}

void UENTCharacterState::StateTick_Implementation(float DeltaTime)
{
	CameraMovement(DeltaTime);
	UpdateViewBobbing(DeltaTime);
}

void UENTCharacterState::StateExit_Implementation(const EENTCharacterStateID& NextStateID){}

#pragma endregion

#pragma region Character

const FPlayerInputs& UENTCharacterState::GetInputs() const
{
	return Controller->GetPlayerInputs();
}

bool UENTCharacterState::IsFalling() const
{
	return Character->GetCharacterMovement()->IsFalling();
}

#pragma endregion

#pragma region Camera

void UENTCharacterState::CameraMovement(float DeltaTime)
{
	if (!bAllowCameraMovement)
	{
		return;
	}

	const UENTSettingsSave* Settings = GetSettings();
	if (!Settings)
	{
		return;
	}

	float TargetYaw = GetInputs().InputLook.X * Settings->MouseSensitivity;
	float TargetPitch = GetInputs().InputLook.Y * Settings->MouseSensitivity * (Settings->bInvertYAxis ? -1 : 1);

	Character->AddControllerYawInput(TargetYaw);
	Character->AddControllerPitchInput(TargetPitch);
}

void UENTCharacterState::UpdateViewBobbing(float DeltaTime)
{
	if (!Character->GetViewBobbingObject() || !GetSettings())
	{
		return;
	}

	const FWaveOscillator CurrentWaveOscillator = Character->GetViewBobbingObject()->GetOscillator();
	const FWaveOscillator TargetWaveOscillator = GetSettings()->bViewBobbing ? ViewBobbing : UENTViewBobbing::GetEmptyOscillator();

	const float CurrentLocationAmplitudeMultiplier = Character->GetViewBobbingObject()->GetLocationAmplitudeMultiplier();
	const float TargetLocationAmplitudeMultiplier = GetSettings()->bViewBobbing ? LocationAmplitudeMultiplier : 0.0f;

	const float CurrentLocationFrequencyMultiplier = Character->GetViewBobbingObject()->GetLocationFrequencyMultiplier();
	const float TargetLocationFrequencyMultiplier = GetSettings()->bViewBobbing ? LocationFrequencyMultiplier : 0.0f;

	const float TargetAmplitude = FMath::Lerp(CurrentWaveOscillator.Amplitude, TargetWaveOscillator.Amplitude, DeltaTime);
	const float TargetFrequency = FMath::Lerp(CurrentWaveOscillator.Frequency, TargetWaveOscillator.Frequency, DeltaTime);

	const float AmplitudeMultiplier = FMath::Lerp(CurrentLocationAmplitudeMultiplier, TargetLocationAmplitudeMultiplier, DeltaTime);
	const float FrequencyMultiplier = FMath::Lerp(CurrentLocationFrequencyMultiplier, TargetLocationFrequencyMultiplier, DeltaTime);

	FWaveOscillator Oscillator;
	Oscillator.Amplitude = TargetAmplitude;
	Oscillator.Frequency = TargetFrequency;
	Oscillator.InitialOffsetType = ViewBobbing.InitialOffsetType;

	Character->GetViewBobbingObject()->SetLocationOscillator(Oscillator, AmplitudeMultiplier, FrequencyMultiplier);
	
	const FWaveOscillator CurrentRollWaveOscillator = Character->GetViewBobbingObject()->GetRollOscillator();

	const UENTCharacterMoveState* MoveState = Cast<UENTCharacterMoveState>(this);

	if (MoveState) LastMoveSpeed = MoveState->GetMoveSpeed();

	const float NormalizedSpeed = (LastMoveSpeed) ? UKismetMathLibrary::NormalizeToRange(Character->GetCharacterMovement()->Velocity.Length(), 0.0f, (MoveState)? MoveState->GetMoveSpeed() : LastMoveSpeed) : 0.0f;

	const float CurrentRotationAmplitudeMultiplier = Character->GetViewBobbingObject()-> GetRotationAmplitudeMultiplier();
	const float TargetRotationAmplitudeMultiplier = GetSettings()->bViewBobbing ? LocationAmplitudeMultiplier * NormalizedSpeed * bAllowRollBobbing * 70.0f : 0.0f;

	float TargetRotationAmplitude = FMath::Lerp(CurrentRollWaveOscillator.Amplitude, TargetWaveOscillator.Amplitude, DeltaTime);
	
	float RotationAmplitudeMultiplier = FMath::Lerp(CurrentRotationAmplitudeMultiplier, TargetRotationAmplitudeMultiplier, DeltaTime);
	
	FWaveOscillator RollOsc;
	RollOsc.Frequency = TargetFrequency;
	RollOsc.Amplitude = TargetRotationAmplitude;
	RollOsc.InitialOffsetType = ViewBobbing.InitialOffsetType;

	Character->GetViewBobbingObject()->SetRollOscillator(RollOsc, RotationAmplitudeMultiplier, FrequencyMultiplier / 8.f);
}

#pragma endregion

#pragma region Noise

void UENTCharacterState::EmitNoise()
{
	if (!bDoesMakeNoise || !bShouldEmitNoises)
	{
		return;
	}

	Character->EmitNoise(NoiseRange, Loudness, Character->GetActorLocation(), NoiseTag);

#if WITH_EDITORONLY_DATA
	if (bDebugState)
	{
		UKismetSystemLibrary::DrawDebugCylinder(Character, Character->GetActorLocation(), Character->GetActorLocation(), NoiseRange, 12, FLinearColor::Red, DrawDebugNoiseDuration);
	}
#endif
}

#pragma endregion

UENTSettingsSave* UENTCharacterState::GetSettings() const
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(Character);
	if (!GameInstance)
	{
		return nullptr;
	}

	UENTSettingsSaveSubsystem* SettingsSubsystem = GameInstance->GetSubsystem<UENTSettingsSaveSubsystem>();
	if (!SettingsSubsystem)
	{
		return nullptr;
	}

	return SettingsSubsystem->GetSettings();
}
