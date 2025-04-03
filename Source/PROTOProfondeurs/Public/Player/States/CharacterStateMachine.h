// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterState.h"
#include "UObject/Object.h"
#include "CharacterStateMachine.generated.h"

enum class ECharacterStateID : uint8;
class UCharacterState;
class AFirstPersonCharacter;

UCLASS()
class PROTOPROFONDEURS_API UCharacterStateMachine final : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TObjectPtr<AFirstPersonCharacter> Character;

	UPROPERTY()
	TArray<TObjectPtr<UCharacterState>> StateList;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	ECharacterStateID CurrentStateID;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UCharacterState> CurrentState;

public:
	void InitStateMachine(AFirstPersonCharacter* InCharacter);

	void TickStateMachine(float DeltaTime);

	AFirstPersonCharacter* GetCharacter() const {return Character;}

	UFUNCTION(BlueprintCallable, Category = "CharacterStateMachine")
	UCharacterState* ChangeState(ECharacterStateID NextStateID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	UCharacterState* FindState(ECharacterStateID StateID) const;

	ECharacterStateID GetCurrentStateID() const {return CurrentStateID;}

	UCharacterState* GetCurrentState() const {return CurrentState;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine", meta = (DeterminesOutputType = "StateClass"))
	UCharacterState* FindStateByClass(TSubclassOf<UCharacterState> StateClass);
};

template<typename StateT = UCharacterState>
inline StateT* FindState(UCharacterStateMachine* StateMachine)
{
	if (!StateMachine)
	{
		return nullptr;
	}

	UCharacterState* State = StateMachine->FindStateByClass(StateT::StaticClass());
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


