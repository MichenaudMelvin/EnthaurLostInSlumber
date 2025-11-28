// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ENTPhysicConstraint.h"
#include "GameElements/ENTWeakZoneInterface.h"
#include "GameFramework/Character.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTDefaultCharacter.generated.h"

class UPostProcessComponent;
class UAkAudioEvent;
class AENTRespawnTree;
class UAkComponent;
class UENTCameraShakeComponent;
class UENTHealthComponent;
enum class EAmberType : uint8;
class UTestViewModel;
class UENTViewBobbing;
class AENTDefaultPlayerController;
class UENTInteractableComponent;
class UENTCharacterStateMachine;
class UAIPerceptionStimuliSourceComponent;
class UENTCharacterState;
enum class EENTCharacterStateID : uint8;
class UCameraComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FKOnAmberUpdate, bool, bHasAmber);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKOnRespawn);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractionFeedback, bool, bCanInteract);

UCLASS()
class ENTCORE_API AENTDefaultCharacter : public ACharacter, public IENTWeakZoneInterface, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AENTDefaultCharacter();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	TObjectPtr<UENTHealthComponent> HealthComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Controller")
	TObjectPtr<AENTDefaultPlayerController> FirstPersonController;

public:
	UCameraComponent* GetCamera() const {return CameraComponent;}

	USkeletalMeshComponent* GetCharacterMesh() const {return CharacterMesh;}

	UENTHealthComponent* GetHealth() const {return HealthComponent;}

	UFUNCTION(BlueprintCallable, Exec, Category = "Health")
	void IgnoreDamages(bool bIgnore = true);

	UFUNCTION(BlueprintCallable)
	UENTCameraShakeComponent* GetCameraShake() const {return ShakeManager;}

#pragma endregion

#pragma region Camera

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UENTViewBobbing> ViewBobbing;

public:
	UENTViewBobbing* GetViewBobbingObject() const {return ViewBobbing;}

#pragma endregion

#pragma region PostProcess

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PostProcess")
	TObjectPtr<UPostProcessComponent> PostProcessComp;

	UPROPERTY(EditDefaultsOnly, Category = "PostProcess")
	TObjectPtr<UMaterialInterface> SpeedEffectMaterialReference;

	UPROPERTY(BlueprintReadOnly, Category = "PostProcess")
	TObjectPtr<UMaterialInstanceDynamic> SpeedEffectMaterial;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PostProcess")
	FName SpeedEffectParamName = "BlurDistance";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PostProcess")
	FFloatRange SpeedEffectVelocityRange = FFloatRange(1000.0f, 5000.0f);

	void UpdateSpeedEffect(float DeltaSeconds);

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Display related speed effect values at screen, editor only
	 */
	UPROPERTY(EditDefaultsOnly, Category = "PostProcess")
	bool bShowSpeedEffectValues = false;
#endif

#pragma endregion

#pragma region StateMachine

protected:
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateMachine")
	TObjectPtr<UENTCharacterStateMachine> StateMachine;

	UPROPERTY(EditDefaultsOnly, Category = "CharacterStateMachine")
	TMap<EENTCharacterStateID, TSubclassOf<UENTCharacterState>> CharacterStates;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly, Category = "CharacterStateMachine")
	TArray<TObjectPtr<UENTCharacterState>> States;

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
	const TArray<TObjectPtr<UENTCharacterState>>& GetStates() const {return States;}

	TObjectPtr<UENTCharacterStateMachine> GetStateMachine() const {return StateMachine;}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "CharacterStateMachine")
	bool CompareCurrentState(EENTCharacterStateID Other) const;

#pragma endregion

#pragma region Interaction

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (Units = "cm"))
	float InteractionTraceLength = 100.0f;

	UPROPERTY()
	float InteractionCoolDown = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UENTInteractableComponent> CurrentInteractable;

	void InteractionTrace();

	void RemoveInteraction();

public:
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractionFeedback OnInteractionFeedback;

	UENTInteractableComponent* GetCurrentInteractable() const {return CurrentInteractable;}

#pragma endregion

#pragma region Ground

protected:
	virtual void Landed(const FHitResult& Hit) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ground")
	bool GroundTrace(const FVector& StartLocation, float TraceLength, FHitResult& HitResult) const;

	bool GroundTrace(FHitResult& HitResult) const {return GroundTrace(GetBottomLocation(), GroundTraceLength, HitResult);}

	bool GroundTrace(const FVector& StartLocation, FHitResult& HitResult) const {return GroundTrace(StartLocation, GroundTraceLength, HitResult);}

	bool GroundTrace(float TraceLength, FHitResult& HitResult) const {return GroundTrace(GetBottomLocation(), TraceLength, HitResult);}

protected:
	void GroundMovement();

	void AboveActor(AActor* ActorBellow);

	UPROPERTY(BlueprintReadOnly, Category = "Ground")
	TObjectPtr<AActor> GroundActor;

	UPROPERTY(EditDefaultsOnly, Category = "Ground", meta = (Units = "cm"))
	float GroundTraceLength = 10.0f;

#pragma endregion

protected:
	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

#pragma region Amber

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Amber")
	bool bHasAmber = false;

public:
	UPROPERTY(BlueprintAssignable, Category = "Amber")
	FKOnAmberUpdate OnAmberUpdate;

	UFUNCTION(BlueprintCallable, Category = "Amber")
	void MineAmber();

	UFUNCTION(BlueprintCallable, Category = "Amber")
	void UseAmber();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Amber")
	bool HasAmber() const {return bHasAmber;}

#if WITH_EDITOR
private:
	UFUNCTION(Exec)
	void IgnoreWeakZone(bool bIgnore) const;
#endif

#pragma endregion

#pragma region CharacterFunctions

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetBottomLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	FVector GetTopLocation() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	float GetCharacterHalfHeight() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
	float GetCharacterHeight() const {return GetCharacterHalfHeight() * 2.0f;}

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

	/**
	 * @brief 
	 * @param Location 
	 * @param Duration Duration in seconds
	 * @param CurveFloat 
	 */
	UFUNCTION(BlueprintCallable, Category = "Character")
	void LookAtLocation(const FVector Location = FVector::ZeroVector, float Duration = 1.0f, UCurveFloat* CurveFloat = nullptr);

#if WITH_EDITOR
	UFUNCTION(Exec)
	void EjectCharacterForward(float Force = 5000.0f) const;
#endif

	UFUNCTION(BlueprintCallable, Category = "Character")
	void StopCharacter() const;

	UFUNCTION(BlueprintCallable, BlueprintPure,Category = "Character")
	bool IsStopped() const;

	AENTDefaultPlayerController* GetPlayerController() const {return FirstPersonController;}

#pragma endregion

#pragma region Saves

private:
	FENTGameElementData EmptyData;

#if WITH_EDITOR
	UFUNCTION(Exec)
	void SavePlayer();
#endif

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

#pragma endregion

#pragma region Respawn

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Respawn")
	TObjectPtr<AENTRespawnTree> LastRespawnTree = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Respawn", meta = (Units = cm, ClampMin = 0.0f))
	float RespawnGroundTrace = 500.0f;

public:
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Respawn")
	FKOnRespawn OnRespawn;

	TObjectPtr<AENTRespawnTree> GetRespawnTree() const {return LastRespawnTree;}

	void SetRespawnTree(AENTRespawnTree* InRespawnTree) {LastRespawnTree = InRespawnTree;}

	void Respawn();

protected:
	virtual void FellOutOfWorld(const UDamageType& dmgType) override;

	UFUNCTION()
	void OnPlayerDie();

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

	/**
	 * @brief 
	 * @param Loudness 
	 * @param NoiseRange 
	 * @param NoiseLocation if equal FVector::ZeroVector will take the pawn location
	 * @param NoiseTag 
	 */
	UFUNCTION(BlueprintCallable, Exec, Category = "Noise")
	void EmitNoise(float NoiseRange = 1000.0f, float Loudness = 1.0f, FVector NoiseLocation = FVector::ZeroVector, const FName& NoiseTag = NAME_None);

#pragma endregion

#pragma region Temp

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConstraintAdded, UENTPhysicConstraint*, Constraint);

	UPROPERTY(BlueprintAssignable, Category = "Constraints")
	FOnConstraintAdded OnConstraintAdded;

	UENTPhysicConstraint* AddConstraint(bool bIsLigament);

#pragma endregion
};
