// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTCharacterState.h"
#include "UObject/Object.h"
#include "ENTCharacterStateMachine.generated.h"

enum class EENTCharacterStateID : uint8;
class UENTCharacterState;
class AENTDefaultCharacter;

UCLASS()
class ENTCORE_API UENTCharacterStateMachine final : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<AENTDefaultCharacter> Character;

	UPROPERTY()
	TArray<TObjectPtr<UENTCharacterState>> StateList;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	EENTCharacterStateID CurrentStateID;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UENTCharacterState> CurrentState;

public:
	void InitStateMachine(AENTDefaultCharacter* InCharacter);

	void TickStateMachine(float DeltaTime);

	AENTDefaultCharacter* GetCharacter() const {return Character;}

	/**
	 * @brief Change a State
	 * @param NextStateID The id of the target next state
	 * @return Return the object of the next state
	 */
	UFUNCTION(BlueprintCallable, Category = "CharacterStateMachine")
	UENTCharacterState* ChangeState(EENTCharacterStateID NextStateID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	UENTCharacterState* FindState(EENTCharacterStateID StateID) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterStateMachine")
	void LockState(EENTCharacterStateID StateToLock, bool bLock) const;

	UFUNCTION(BlueprintCallable, Category = "CharacterStateMachine")
	void LockAllStates(bool bLock);

	EENTCharacterStateID GetCurrentStateID() const {return CurrentStateID;}

	UENTCharacterState* GetCurrentState() const {return CurrentState;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine", meta = (DeterminesOutputType = "StateClass"))
	UENTCharacterState* FindStateByClass(TSubclassOf<UENTCharacterState> StateClass);
};

template<typename StateT = UENTCharacterState>
inline StateT* FindState(UENTCharacterStateMachine* StateMachine)
{
	if (!StateMachine)
	{
		return nullptr;
	}

	UENTCharacterState* State = StateMachine->FindStateByClass(StateT::StaticClass());
	if (!State)
	{
		return nullptr;
	}

	StateT* CastState = Cast<StateT>(State);
	if (!CastState)
	{
		return nullptr;
	}

	return CastState;
}


