// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PROTOProfondeurs/Public/GameElements/WeakZoneInterface.h"
#include "GameFramework/Character.h"
#include "FirstPersonCharacter.generated.h"

class UCameraShakeComponent;
enum class EAmberType : uint8;
class UTestViewModel;
class UViewBobbing;
class AFirstPersonController;
class UInteractableComponent;
class UCharacterStateMachine;
class UAIPerceptionStimuliSourceComponent;
class UCharacterState;
enum class ECharacterStateID : uint8;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKOnAmberUpdate, EAmberType, AmberType, int, AmberAmount);

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UCameraShakeComponent> ShakeManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<AFirstPersonController> FirstPersonController;

	UPROPERTY(EditDefaultsOnly, Category = "Stimuli")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> HearingStimuli;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spike")
	TObjectPtr<USkeletalMeshComponent> SpikeMesh;

public:
	UCameraComponent* GetCamera() const {return CameraComponent;}
	USkeletalMeshComponent* GetCharacterMesh() const {return CharacterMesh;}
	UCameraShakeComponent* GetCameraShake() const {return ShakeManager;}

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
	TArray<TObjectPtr<UCharacterState>> States;

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
	const TArray<TObjectPtr<UCharacterState>>& GetStates() const {return States;}

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (Units = "cm"))
	float InteractionTraceLength = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UInteractableComponent> CurrentInteractable;

	void InteractionTrace();

	void RemoveInteraction();

public:
	UInteractableComponent* GetCurrentInteractable() const {return CurrentInteractable;}

	void SetInteractionUI(bool bState) const;

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
	UPROPERTY(BlueprintReadOnly, Category = "Amber")
	TMap<EAmberType, int> AmberInventory;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TMap<EAmberType, int> AmberInventoryMaxCapacity;

public:
	UPROPERTY(BlueprintAssignable, Category = "Amber")
	FKOnAmberUpdate OnAmberUpdate;

	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Amber")
	void MineAmber(const EAmberType& AmberType, const int Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Amber")
	void UseAmber(const EAmberType& AmberType, const int Amount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Amber")
	bool IsAmberTypeFilled(const EAmberType& AmberType) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Amber")
	bool HasRequiredQuantity(const EAmberType& AmberType, const int Quantity) const;

#pragma endregion

#pragma region Spike

protected:
	FTransform SpikeRelativeTransform;

	UPROPERTY()
	TObjectPtr<USceneComponent> SpikeParent;

	FTransform SpikeTargetTransform;

	bool bUseSpikeRelativeTransform = true;

	UPROPERTY(EditDefaultsOnly, Category = "Spike", meta = (UIMin = 0.0f))
	float SpikeLerpSpeed = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Spike", meta = (UIMin = 0.0f, Units = "cm"))
	float SpikeChargingOffset = 25.0f;

	void UpdateSpikeLocation(float DeltaTime) const;

public:
	void PlantSpike(const FVector& TargetLocation);

	void ReGrabSpike();

	void UpdateSpikeOffset(float Alpha) const;

#pragma endregion

#pragma region CharacterFunctions

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetBottomLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetTopLocation() const;

	/**
	 * @brief 
	 * @param TopLocation true will return the top location of the player, false will return the bottom location
	 * @return The return location
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetPlayerLocation(bool TopLocation) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	bool GetSlopeProperties(float& SlopeAngle, FVector& SlopeNormal) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void EjectCharacter(const FVector ProjectionVelocity) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void StopCharacter() const;

	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Character")
	bool IsStopped() const;

	AFirstPersonController* GetPlayerController() const {return FirstPersonController;}

#pragma endregion

#pragma region Respawn

private:
	FVector RespawnPosition;

public:
	FVector GetRespawnPosition() const {return RespawnPosition;}

	void SetRespawnPosition(const FVector& Position) {RespawnPosition = Position;}

#pragma endregion

protected:
	TObjectPtr<UTestViewModel> ViewModel;

#pragma region Debug

#if WITH_EDITORONLY_DATA

private:
	UFUNCTION(Exec)
	void ChangeState(const ECharacterStateID& State) const;

#endif

#pragma endregion
};
