// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shakes/WaveOscillatorCameraShakePattern.h"
#include "UObject/Object.h"
#include "CharacterState.generated.h"

class USettingsSave;
class UViewBobbing;
struct FPlayerInputs;
class AFirstPersonController;
class AFirstPersonCharacter;
class UCharacterStateMachine;

UENUM(BlueprintType)
enum class ECharacterStateID : uint8
{
	None,
	Idle,
	Walk,
	Sprint,
	Crouch,
	Jump,
	Fall,
	Interact,
	Slide,
	TakeAmber,
};

/**
 * @brief Base class for the character state machine
 */
UCLASS(Abstract, Blueprintable)
class PROTOPROFONDEURS_API UCharacterState : public UObject
{
	GENERATED_BODY()

#pragma region States
public:
	UCharacterState();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UCharacterStateMachine> StateMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterStateMachine")
	ECharacterStateID StateID = ECharacterStateID::None;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug", meta = (ToolTip = "Enable debug features for the current state"))
	bool bDebugState = false;
#endif

public:
	const ECharacterStateID& GetStateID() const {return StateID;}

	virtual void StateInit(UCharacterStateMachine* InStateMachine);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateEnter(const ECharacterStateID& PreviousStateID);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateTick(float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateExit(const ECharacterStateID& NextStateID);

#pragma endregion

#pragma region Character

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AFirstPersonCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
	TObjectPtr<AFirstPersonController> Controller;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	const FPlayerInputs& GetInputs() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool IsFalling() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Ground")
	bool bCheckGround = false;

	UFUNCTION(BlueprintCallable, Category = "Character|Ground")
	void CheckGround();

	UPROPERTY(BlueprintReadOnly, Category = "Character|Ground")
	TEnumAsByte<EPhysicalSurface> CurrentSurface = SurfaceType1;

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Ground")
	void OnWalkOnNewSurface(const TEnumAsByte<EPhysicalSurface>& NewSurface);

#pragma endregion

#pragma region Camera

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bAllowCameraMovement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	FWaveOscillator ViewBobbing;

	UPROPERTY(EditDefaultsOnly, Category = "Camera", DisplayName = "FOV", meta = (ClampMin = 5.0f, ClampMax = 170.0f, UIMin = 5.0f, UIMax = 170.0f))
	float TargetFOV = 90.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (ClampMin = 0.0f, ClampMax = 90.0f, UIMin = 0.0f, UIMax = 90.0f))
	float MaxSteering = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Camera", meta = (ClampMin = 0.01f, ClampMax = 10.0f, UIMin = 0.01f, UIMax = 10.0f))
	float SteeringSpeed = 1.0f;

private:
	void CameraMovement(float DeltaTime);

	void UpdateCameraFOV(float DeltaTime);

	void UpdateViewBobbing(float DeltaTime);

protected:
	float TargetSteering = 0.0f;

	virtual void UpdateCameraSteering(float DeltaTime);

#pragma endregion

protected:
	USettingsSave* GetSettings() const;
};
