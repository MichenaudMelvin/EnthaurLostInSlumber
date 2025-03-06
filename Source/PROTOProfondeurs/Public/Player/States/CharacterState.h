// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterState.generated.h"

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

UCLASS(Abstract, Blueprintable)
class PROTOPROFONDEURS_API UCharacterState : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<AFirstPersonCharacter> Character;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<AFirstPersonController> Controller;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UCharacterStateMachine> StateMachine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterStateMachine")
	ECharacterStateID StateID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	bool bAllowCameraMovement = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TSubclassOf<UViewBobbing> ViewBobbing;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UViewBobbing> CurrentViewBobbing;

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void StartCameraShake();

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void StopCameraShake();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	const FPlayerInputs& GetInputs() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool IsFalling() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Ground")
	bool bCheckGround = false;

	UFUNCTION(BlueprintCallable, Category = "Character|Ground")
	void CheckGround();

	UPROPERTY(BlueprintReadOnly, Category = "Character|Ground")
	TEnumAsByte<EPhysicalSurface> CurrentSurface;

	UFUNCTION(BlueprintNativeEvent, Category = "Character|Ground")
	void OnWalkOnNewSurface(const TEnumAsByte<EPhysicalSurface>& NewSurface);

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
};
