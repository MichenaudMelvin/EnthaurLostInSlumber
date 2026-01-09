// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/ENTSavedActor.h"
#include "ENTBehaviorTrigger.generated.h"

class UBoxComponent;
class AENTDefaultAIController;

/**
 * @brief For now this class only run a behavior tree when an actor with a UAIPerceptionStimuliSourceComponent enter the triggerBox
 */
UCLASS()
class ENTARTIFICIALINTELLIGENCE_API AENTBehaviorTrigger : public AENTSavedActor
{
	GENERATED_BODY()

public:
	AENTBehaviorTrigger();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defaults")
	TObjectPtr<USceneComponent> RootComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trigger")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, Category = "Trigger")
	FVector BoxSize = FVector(100.0f);

	UFUNCTION()
	void OnEnterTriggerBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/*
	UPROPERTY(EditInstanceOnly, Category = "Behavior")
	FName BlackboardParamName;
	*/

	UPROPERTY(EditAnywhere, Category = "Behavior")
	bool bDestroyWhenTrigger = true;

	UPROPERTY(EditInstanceOnly, Category = "Behavior")
	TObjectPtr<APawn> AIToTrigger;

	UPROPERTY()
	TObjectPtr<AENTDefaultAIController> AIController;

#pragma region Saves
public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

	virtual void FinishLoading(UENTWorldSave* LoadedWorldSave) override;

#pragma endregion
};
