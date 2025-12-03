// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interface/ENTActivation.h"
#include "Interfaces/ENTPawnAIInterface.h"
#include "Path/ENTArtificialIntelligencePath.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTParasitePawn.generated.h"

class UENTHealthComponent;
class AENTNavigationArea;
class AENTParasiteController;
class UENTGravityPawnMovement;
class AENTArtificialIntelligencePath;
class UCapsuleComponent;
class UBoxComponent;
class UAIPerceptionComponent;
struct FENTAIData;
struct FENTGameElementData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChangeAnimToTrigger, UAnimSequenceBase*, AnimToTrigger);

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTParasitePawn : public APawn, public IENTSaveGameElementInterface, public IENTPawnAIInterface, public IENTActivation
{
	GENERATED_BODY()

public:
	AENTParasitePawn();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void Tick(float DeltaSeconds) override;
#endif

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Respawn")
	void RespawnParasite();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCapsuleComponent> ParasiteCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UENTGravityPawnMovement> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ParasiteMesh;

	UPROPERTY(EditInstanceOnly, Category = "Mesh")
	bool bOverrideDefaultRotation = false;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> ForwardDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> LeftDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> UpDirection;
#endif

	UPROPERTY(BlueprintReadOnly, Category = "AI");
	TObjectPtr<AENTParasiteController> ParasiteController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AENTArtificialIntelligencePath> TargetPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AENTNavigationArea> NavigationArea;

#pragma region BlackboardKeys

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName ResetBehaviorKeyName = "ResetBehavior";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName SpawnLocationKeyName = "SpawnLocation";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName SpawnRotationKeyName = "SpawnRotation";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName PathKeyName = "AIPath";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName NavAreaKeyName = "NavigationArea";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName WalkOnFloorKeyName = "WalkOnFloor";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName AttackTargetKeyName = "AttackTarget";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName PatrolSpeedKeyName = "PatrolSpeed";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName ChaseSpeedKeyName = "ChaseSpeed";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName DetectionRangeKeyName = "DetectionRange";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName PlayerKeyName = "Player";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName DoesPlayerHaveAmberKeyName = "DoesPlayerHaveAmber";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Blackboard")
	FName UseNavMeshKeyName = "UseNavMesh";

#pragma endregion

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ExposeOnSpawn = true))
	bool bAutoStartBehavior = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ExposeOnSpawn = true))
	bool bUseNavMesh = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ExposeOnSpawn = true))
	bool bAllowRespawn = true;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ExposeOnSpawn = true))
	TObjectPtr<UBehaviorTree> OverridenBehaviorTree = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior", meta = (Units = "cm/s", ExposeOnSpawn = true))
	float PatrolSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior", meta = (Units = "cm/s", ExposeOnSpawn = true))
	float ChaseSpeed = 1200.0f;

#pragma region BehaviorTree

protected:
	virtual bool DoesAutoStartBehaviorTree_Implementation() const override {return bAutoStartBehavior;}

	virtual UBehaviorTree* GetOverridenBehaviorTree_Implementation() const override {return OverridenBehaviorTree;}

	virtual void OnBehaviorTreeStarted_Implementation() override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(BlueprintCallable, Category = "BehaviorTree")
	void StartBehaviorTree();

	virtual void Trigger_Implementation() override;

#pragma endregion

#pragma region DetectionRange

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Detection Range", meta = (Units = cm, ClampMin = 0.0f))
	float DefaultDetectionRange = 1000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Detection Range", meta = (Units = cm, ClampMin = 0.0f))
	float AugmentedDetectionRange = 5000.0f;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Detection Range")
	bool bDebugDetectionRange = false;

	/**
	 * @brief Call this in a tick to display the detection range; Editor Only
	 */
	void DrawDetectionRange() const;
#endif

public:
	UFUNCTION()
	void ChangeDetectionRange(bool bDoesPlayerHaveAmber);

#pragma endregion

#pragma region ParasiteAttack

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Attack")
	float AttackDamages = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Attack")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsToAttack;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "AI|Attack")
	bool bDebugAttack = false;
#endif

	UPROPERTY(BlueprintReadOnly, Category = "AI|Attack")
	TSet<TObjectPtr<UENTHealthComponent>> FoundedHealthComp;

	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void QueryForAttack(const FVector& AttackLocation, const FVector& AttackExtent);

	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void Attack();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "AI|Attack")
	bool SucceedAttack() const;

	/**
	 * @brief For debug purposes only
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Attack", meta = (DevelopmentOnly))
	static void DebugAttackZone(const UObject* WorldContextObject, const FVector& AttackLocation, const FVector& AttackExtent, const FRotator& Rotation, float Duration = 0);

#pragma endregion

#if WITH_EDITORONLY_DATA
protected:
	virtual void DebugPawn() const override;
#endif

#pragma region MathFunctions

protected:
#if WITH_EDITORONLY_DATA
	/**
	 * @brief This is an editor value, please use AENTParasitePawn::GetParasiteHeight() instead
	 */
	UPROPERTY(VisibleDefaultsOnly, Category = "Transformation", meta = (Units = cm))
	float ParasiteHeight = 0.0f;

	/**
	 * @brief This is an editor value, please use AENTParasitePawn::GetParasiteWidth() instead
	 */
	UPROPERTY(VisibleDefaultsOnly, Category = "Transformation", meta = (Units = cm))
	float ParasiteWidth = 0.0f;
#endif

public:
	/**
	 * @brief Return the height of the collision
	 * @return Height of the parasite
	 */
	UFUNCTION(BlueprintCallable, Category = "Transformation")
	float GetParasiteHeight() const;

	/**
	 * @brief Return the half height of the collision
	 * @return Half height of the parasite
	 */
	UFUNCTION(BlueprintCallable, Category = "Transformation")
	float GetParasiteHalfHeight() const {return GetParasiteHeight() * 0.5f;}

	/**
	 * @brief Return the width of the collision
	 * @return Width of the parasite
	 */
	UFUNCTION(BlueprintCallable, Category = "Transformation")
	float GetParasiteWidth() const;

	/**
	 * @brief Return the half width of the collision
	 * @return Half width of the parasite
	 */
	UFUNCTION(BlueprintCallable, Category = "Transformation")
	float GetParasiteHalfWidth() const {return GetParasiteWidth() * 0.5f;}

	UFUNCTION(BlueprintCallable, Category = "Transformation")
	FVector GetParasiteForwardVector() const;

	UFUNCTION(BlueprintCallable, Category = "Transformation")
	FVector GetParasiteRightVector() const;

	UFUNCTION(BlueprintCallable, Category = "Transformation")
	FVector GetParasiteUpVector() const;

#pragma endregion

#pragma region Animations

protected:
	UPROPERTY()
	TObjectPtr<UAnimSequenceBase> AnimToTrigger;

	virtual void SetAnimToTrigger(UAnimSequenceBase* Anim) override;

	UPROPERTY(BlueprintAssignable, Category = "Animation")
	FOnChangeAnimToTrigger OnChangeAnimToTrigger;

#pragma endregion

#pragma region Velocity

protected:
	bool bOverrideVelocity = false;

	FVector OverridenVelocity = FVector::ZeroVector;

public:
	virtual FVector GetVelocity() const override;

	/**
	 * @brief Use it to override the APawn::GetVelocity() function
	 * @param bOverride if true OverridenVelocity is equal to FVector::ForwardVector, else will be FVector::ZeroVector
	 */
	void OverrideVelocity(bool bOverride);

	/**
	 * @brief Use AENTParasitePawn::OverrideVelocity(bool) for clarity purpose and if your don't care about the OverridenVelocity
	 * @param NewVelocity if equal to FVector::ZeroVector bOverrideVelocity will become false
	 */
	void OverrideVelocity(const FVector& NewVelocity);

#pragma endregion

#pragma region Save

protected:
	bool bHasReceivedLoadingRequest = false;

	FENTParasiteData LoadingData;

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

	virtual bool HasReceivedLoadingRequest() const override {return bHasReceivedLoadingRequest;}

	virtual const FENTAIData& GetLoadingData() const override {return LoadingData;}

#pragma endregion

#pragma region DebugSelection

#if WITH_EDITORONLY_DATA
protected:
	void OnSelectionUpdate(UObject* Object);

	void ClearDebugTraces() const;

	bool SelectedInEditor = false;

#endif

#pragma endregion
};
