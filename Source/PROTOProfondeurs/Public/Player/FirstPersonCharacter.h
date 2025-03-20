// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Environement/WeakZoneInterface.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"

class UTestViewModel;
class UViewBobbing;
class AFirstPersonController;
class UInteractableComponent;
class UCharacterStateMachine;
class UCharacterState;
enum class ECharacterStateID : uint8;
class UCameraComponent;

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonCharacter : public ACharacter, public IWeakZoneInterface
{
	GENERATED_BODY()

public:
	AFirstPersonCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Components

protected:
	/** First person camera */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	/** Character mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<AFirstPersonController> FirstPersonController;

public:
	UCameraComponent* GetCamera() const {return CameraComponent;}

	USkeletalMeshComponent* GetCharacterMesh() const {return CharacterMesh;}

#pragma endregion

#pragma region Camera

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UViewBobbing> ViewBobbing;

public:
	UViewBobbing* GetViewBobbingObject() const {return ViewBobbing;}

#pragma endregion

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
#endif

#if WITH_EDITOR
	UFUNCTION(Exec)
	void DisplayStates(bool bDisplay);

public:
	bool DebugStates() const {return bDebugStates;}
#endif

public:
	const TArray<UCharacterState*>& GetStates() const {return States;}

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (Units = "cm"))
	float InteractionTraceLength = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UInteractableComponent> CurrentInteractable;

	void InteractionTrace();

public:
	UInteractableComponent* GetCurrentInteractable() const {return CurrentInteractable;}

#pragma endregion

#pragma region Ground

protected:
	virtual void Landed(const FHitResult& Hit) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ground")
	bool GroundTrace(FHitResult& HitResult) const;

protected:
	void GroundMovement();

	void AboveActor(AActor* ActorBellow);

	UPROPERTY(BlueprintReadOnly, Category = "Ground")
	TObjectPtr<AActor> GroundActor;

	UPROPERTY(EditDefaultsOnly, Category = "Ground", meta = (Units = "cm"))
	float GroundTraceLength = 10.0f;

#pragma endregion

#pragma region Amber

protected:
	bool bCanTakeAmber = false;

	UPROPERTY(BlueprintReadOnly, Category = "Amber")
	bool bAmberFilled = false;

public:
	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

	void FillAmber(bool bRefill) {bAmberFilled = bRefill;}

	bool IsAmberFilled() const {return bAmberFilled;}

#pragma endregion

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetBottomLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetTopLocation() const;

	/**
	 * @brief 
	 * @param TopLocation true will return the top location of the player, false will return the bottom location
	 * @return 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetPlayerLocation(bool TopLocation) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool GetSlopeProperties(float& SlopeAngle, FVector& SlopeNormal) const;

	AFirstPersonController* GetPlayerController() const {return FirstPersonController;}

	void SetInteractionUI(bool bState) const;

protected:
	TObjectPtr<UTestViewModel> ViewModel;
};
