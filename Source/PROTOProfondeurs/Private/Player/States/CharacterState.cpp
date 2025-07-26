// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/States/CharacterState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonController.h"
#include "Player/Camera/ViewBobbing.h"
#include "Player/States/CharacterStateMachine.h"
#include "Saves/ENTSettingsSave.h"
#include "Subsystems/ENTSettingsSaveSubsystem.h"

#pragma region States

UCharacterState::UCharacterState()
{
	ViewBobbing = UViewBobbing::GetEmptyOscillator();
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

void UCharacterState::StateEnter_Implementation(const ECharacterStateID& PreviousStateID) {}

void UCharacterState::StateTick_Implementation(float DeltaTime)
{
	CameraMovement(DeltaTime);
	UpdateViewBobbing(DeltaTime);
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

#pragma endregion

#pragma region Camera

void UCharacterState::CameraMovement(float DeltaTime)
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

void UCharacterState::UpdateViewBobbing(float DeltaTime)
{
	if (!Character->GetViewBobbingObject() || !GetSettings())
	{
		return;
	}

	const FWaveOscillator CurrentWaveOscillator = Character->GetViewBobbingObject()->GetOscillator();
	const FWaveOscillator TargetWaveOscillator = GetSettings()->bViewBobbing ? ViewBobbing : UViewBobbing::GetEmptyOscillator();

	const float CurrentLocationAmplitudeMultiplier = Character->GetViewBobbingObject()->GetLocationAmplitudeMultiplier();
	const float TargetLocationAmplitudeMultiplier = GetSettings()->bViewBobbing ? LocationAmplitudeMultiplier : 0.0f;

	const float CurrentLocationFrequencyMultiplier = Character->GetViewBobbingObject()->GetLocationFrequencyMultiplier();
	const float TargetLocationFrequencyMultiplier = GetSettings()->bViewBobbing ? LocationFrequencyMultiplier : 0.0f;

	float TargetAmplitude = FMath::Lerp(CurrentWaveOscillator.Amplitude, TargetWaveOscillator.Amplitude, DeltaTime);
	float TargetFrequency = FMath::Lerp(CurrentWaveOscillator.Frequency, TargetWaveOscillator.Frequency, DeltaTime);

	float AmplitudeMultiplier = FMath::Lerp(CurrentLocationAmplitudeMultiplier, TargetLocationAmplitudeMultiplier, DeltaTime);
	float FrequencyMultiplier = FMath::Lerp(CurrentLocationFrequencyMultiplier, TargetLocationFrequencyMultiplier, DeltaTime);

	FWaveOscillator Oscillator;
	Oscillator.Amplitude = TargetAmplitude;
	Oscillator.Frequency = TargetFrequency;
	Oscillator.InitialOffsetType = ViewBobbing.InitialOffsetType;

	Character->GetViewBobbingObject()->SetOscillator(Oscillator, AmplitudeMultiplier, FrequencyMultiplier);
}

#pragma endregion

#pragma region Noise

void UCharacterState::EmitNoise() const
{
	if (!bDoesMakeNoise)
	{
		return;
	}

	Character->MakeNoise(Loudness, Character, Character->GetActorLocation(), NoiseRange, NoiseTag);
}

#pragma endregion

UENTSettingsSave* UCharacterState::GetSettings() const
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


