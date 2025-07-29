// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PROTOProfondeurs/Public/GameElements/WeakZoneInterface.h"
#include "GameFramework/Character.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "FirstPersonCharacter.generated.h"

class UAkAudioEvent;
class ARespawnTree;
class UAkComponent;
class UENTCameraShakeComponent;
class UENTHealthComponent;
enum class EAmberType : uint8;
class UTestViewModel;
class UViewBobbing;
class AFirstPersonController;
class UENTInteractableComponent;
class UCharacterStateMachine;
class UAIPerceptionStimuliSourceComponent;
class UCharacterState;
enum class ECharacterStateID : uint8;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKOnAmberUpdate, EAmberType, AmberType, int, AmberAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKOnRespawn);

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonCharacter : public ACharacter, public IWeakZoneInterface, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AFirstPersonCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Components

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UENTCameraShakeComponent> ShakeManager;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stimuli")
	TObjectPtr<UAIPerceptionStimuliSourceComponent> HearingStimuli;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spike")
	TObjectPtr<USkeletalMeshComponent> SpikeMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UENTHealthComponent> HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<AFirstPersonController> FirstPersonController;

public:
	UCameraComponent* GetCamera() const {return CameraComponent;}
	USkeletalMeshComponent* GetCharacterMesh() const {return CharacterMesh;}
	UENTCameraShakeComponent* GetCameraShake() const {return ShakeManager;}

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

	TObjectPtr<UCharacterStateMachine> GetStateMachine() const {return StateMachine;}

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (Units = "cm"))
	float InteractionTraceLength = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UENTInteractableComponent> CurrentInteractable;

	void InteractionTrace();

	void RemoveInteraction();

public:
	UENTInteractableComponent* GetCurrentInteractable() const {return CurrentInteractable;}

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
	void EjectCharacter(const FVector ProjectionVelocity, bool bOverrideCurrentVelocity) const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void StopCharacter() const;

	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Character")
	bool IsStopped() const;

	AFirstPersonController* GetPlayerController() const {return FirstPersonController;}

#pragma endregion

#pragma region Saves

private:
	FENTGameElementData EmptyData;

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData) override;

#pragma endregion

#pragma region Respawn

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Respawn")
	TObjectPtr<ARespawnTree> LastRespawnTree = nullptr;

	UFUNCTION()
	void OnPlayerDie();

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Respawn")
	FKOnRespawn OnRespawn;

	TObjectPtr<ARespawnTree> GetRespawnTree() const {return LastRespawnTree;}

	void SetRespawnTree(ARespawnTree* InRespawnTree) {LastRespawnTree = InRespawnTree;}

	void Respawn(const FTransform& RespawnTransform);

#pragma endregion

#pragma region Sounds

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sounds")
	TObjectPtr<UAkComponent> FootstepsSounds;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	TObjectPtr<UAkAudioEvent> LandedEvent;

	UPROPERTY()
	TObjectPtr<UAkAudioEvent> DefaultFootStepEvent;

public:
	TObjectPtr<UAkComponent> GetFootstepsSoundComp() const {return FootstepsSounds;}

	void ResetFootStepsEvent() const;

#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> StartWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UUserWidget> StartWidget;

public:
	TObjectPtr<UUserWidget> GetStartWidget() { return StartWidget; }
};
