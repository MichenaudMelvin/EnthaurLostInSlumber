// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"

class UCharacterStateMachine;
class UCharacterState;
enum class ECharacterStateID : uint8;
class UCameraComponent;

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFirstPersonCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	/** First person camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	/** Character mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

#pragma region StateMachine

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UCharacterStateMachine> StateMachine;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterStateMachine")
	TMap<ECharacterStateID, TSubclassOf<UCharacterState>> CharacterStates;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "CharacterStateMachine")
	TArray<UCharacterState*> States;

	void InitStateMachine();

	void TickStateMachine(float DeltaTime);

	void CreateStates();

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditDefaultsOnly, Category = "CharacterStateMachine|Debug")
	bool bDebugStates = false;

	UFUNCTION(Exec)
	void DisplayStates(bool bDisplay);

public:
	bool DebugStates() const {return bDebugStates;}
#endif

public:
	const TArray<UCharacterState*>& GetStates() const {return States;}

#pragma endregion 
};
