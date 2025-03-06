// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	TArray<UCharacterState*> StateList;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	ECharacterStateID CurrentStateID;

	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UCharacterState> CurrentState;

public:
	void InitStateMachine(AFirstPersonCharacter* InCharacter);

	void TickStateMachine(float DeltaTime);

	AFirstPersonCharacter* GetCharacter() const {return Character;}

	UFUNCTION(BlueprintCallable, Category = "CharacterStateMachine")
	void ChangeState(ECharacterStateID NextStateID);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	UCharacterState* FindState(ECharacterStateID StateID) const;

	ECharacterStateID GetCurrentStateID() const {return CurrentStateID;}

	UCharacterState* GetCurrentState() const {return CurrentState;}

private:
	/**
	 * @brief BlueprintOnly
	 * @param StateClass 
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine", meta = (DeterminesOutputType = "StateClass"))
	UCharacterState* FindStateByClass(TSubclassOf<UCharacterState> StateClass);
};
