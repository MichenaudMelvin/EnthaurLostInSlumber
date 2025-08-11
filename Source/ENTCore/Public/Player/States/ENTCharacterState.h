// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"
#include "UObject/Object.h"
#include "ENTCharacterState.generated.h"

class UENTSettingsSave;
class UENTViewBobbing;
struct FPlayerInputs;
class AENTDefaultPlayerController;
class AENTDefaultCharacter;
class UENTCharacterStateMachine;

UENUM(BlueprintType)
enum class EENTCharacterStateID : uint8
{
	None,
	Idle,
	Walk,
	Sprint,
	Crouch,
	Jump,
	Fall,
	Interact,
	Stop,
	Anim,
};

/**
 * @brief Base class for the character state machine
 */
UCLASS(Abstract, Blueprintable)
class ENTCORE_API UENTCharacterState : public UObject
{
	GENERATED_BODY()

#pragma region States

public:
	UENTCharacterState();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UENTCharacterStateMachine> StateMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterStateMachine")
	EENTCharacterStateID StateID = EENTCharacterStateID::None;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterStateMachine")
	bool bLockState = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "Enable debug features for the current state"))
	bool bDebugState = false;
#endif

public:
	const EENTCharacterStateID& GetStateID() const {return StateID;}

	virtual void StateInit(UENTCharacterStateMachine* InStateMachine);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateEnter(const EENTCharacterStateID& PreviousStateID);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateTick(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateExit(const EENTCharacterStateID& NextStateID);

	void LockState(bool bInLockState) {bLockState = bInLockState;}

	bool IsLocked() const {return bLockState;}

#pragma endregion

#pragma region Character

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TObjectPtr<UAnimSequenceBase> StateAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AENTDefaultCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AENTDefaultPlayerController> Controller;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	const FPlayerInputs& GetInputs() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool IsFalling() const;

public:
	void SetStateAnimation(UAnimSequenceBase* InStateAnimation) {StateAnimation = InStateAnimation;}

#pragma endregion

#pragma region Camera

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	bool bAllowCameraMovement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|ViewBobbing")
	FWaveOscillator ViewBobbing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|ViewBobbing")
	float LocationAmplitudeMultiplier = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera|ViewBobbing")
	float LocationFrequencyMultiplier = 2.5f;

private:
	void CameraMovement(float DeltaTime);

	void UpdateViewBobbing(float DeltaTime);

#pragma endregion

#pragma region Noise

protected:
	/**
	 * @brief Noise is for AI
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise")
	bool bDoesMakeNoise = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise", meta = (EditCondition = bDoesMakeNoise, ClampMin = 0.0f, Units = "cm"))
	float NoiseRange = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise", meta = (ClampMin = 0.0f, ClampMax = 1.0f, UIMin = 0.0f, UIMax = 1.0f, EditCondition = bDoesMakeNoise))
	float Loudness = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Noise", meta = (EditCondition = bDoesMakeNoise))
	FName NoiseTag;

	void EmitNoise() const;

#pragma endregion

protected:
	UENTSettingsSave* GetSettings() const;
};
