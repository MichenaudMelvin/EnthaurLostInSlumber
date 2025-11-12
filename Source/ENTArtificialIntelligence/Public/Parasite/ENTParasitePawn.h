// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/ENTPawnAIInterface.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTParasitePawn.generated.h"

class AENTNavigationArea;
class AENTParasiteController;
class UENTGravityPawnMovement;
class AENTArtificialIntelligencePath;
class UBoxComponent;
class UAIPerceptionComponent;
struct FENTAIData;
struct FENTGameElementData;

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTParasitePawn : public APawn, public IENTSaveGameElementInterface, public IENTPawnAIInterface
{
	GENERATED_BODY()

public:
	AENTParasitePawn();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ParasiteCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UENTGravityPawnMovement> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ParasiteMesh;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> ForwardDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> LeftDirection;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> UpDirection;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ParasiteDeathZone;

	UPROPERTY(BlueprintReadOnly, Category = "AI");
	TObjectPtr<AENTParasiteController> ParasiteController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AENTArtificialIntelligencePath> TargetPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AENTNavigationArea> NavigationArea;

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

	UPROPERTY(EditInstanceOnly, Category = "AI|Behavior")
	bool bAutoStartBehavior = true;

	UPROPERTY(EditInstanceOnly, Category = "AI|Behavior")
	TObjectPtr<UBehaviorTree> OverridenBehaviorTree = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Behavior", meta = (Units = "cm/s"))
	float PatrolSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI|Behavior", meta = (Units = "cm/s"))
	float ChaseSpeed = 1200.0f;

	virtual bool DoesAutoStartBehaviorTree_Implementation() const override {return bAutoStartBehavior;}

	virtual UBehaviorTree* GetOverridenBehaviorTree_Implementation() const override {return OverridenBehaviorTree;}

	virtual void OnBehaviorTreeStarted_Implementation() override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void EnterDeathZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

#if WITH_EDITORONLY_DATA
	virtual void DebugPawn() const override;
#endif

public:
	UBoxComponent* GetCollisionComp() {return ParasiteCollision;}

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
};
