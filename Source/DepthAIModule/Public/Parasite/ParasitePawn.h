// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Saves/WorldSaves/ENTGameElementData.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ParasitePawn.generated.h"

class AParasiteController;
class UGravityPawnMovement;
class AAIPath;
class UBoxComponent;
class UAIPerceptionComponent;

UCLASS()
class DEPTHAIMODULE_API AParasitePawn : public APawn, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AParasitePawn();

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
	TObjectPtr<UGravityPawnMovement> MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
	TObjectPtr<USkeletalMeshComponent> ParasiteMesh;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
	TObjectPtr<class UArrowComponent> ForwardDirection;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ParasiteDeathZone;

	UPROPERTY(BlueprintReadOnly, Category = "AI");
	TObjectPtr<AParasiteController> ParasiteController;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	TObjectPtr<AAIPath> TargetPath;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName PathKeyName = "AIPath";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName WalkOnFloorKeyName = "WalkOnFloor";

	UPROPERTY(EditDefaultsOnly, Category = "AI|Blackboard")
	FName AttackTargetKeyName = "AttackTarget";

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	void EnterDeathZone(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UBoxComponent* GetCollisionComp() {return ParasiteCollision;}


#pragma region Save

protected:
	bool bLoadBlackboardData = false;

	FENTParaSiteData BlackboardData;

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData) override;

#pragma endregion
};
