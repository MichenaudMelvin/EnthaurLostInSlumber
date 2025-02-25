// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterState.generated.h"

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

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	bool bAllowCameraMovement = true;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	const FPlayerInputs& GetInputs() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool IsFalling() const;

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
	void StateExit(const ECharacterStateID& NextStateID);

	UFUNCTION(BlueprintNativeEvent, Category = "CharacterStateMachine")
	void StateTick(float DeltaTime);
};
