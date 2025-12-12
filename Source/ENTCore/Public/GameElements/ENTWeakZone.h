// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTAmberOre.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTWeakZone.generated.h"

class ALight;
class UENTElectricityComponent;
class UAkAudioEvent;

class UENTInteractableComponent;
class UBoxComponent;
class UPostProcessComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCure);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCorrupt);

UCLASS()
class ENTCORE_API AENTWeakZone : public AActor, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AENTWeakZone();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostLoad() override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeakZone")
	TObjectPtr<UPostProcessComponent> BlackAndWhiteShader;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeakZone")
	TObjectPtr<UPostProcessComponent> WeakZonePostProcess;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Electricity")
	TObjectPtr<UENTElectricityComponent> ElectricityComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity")
	FLinearColor ElectricityCureColor;

	UPROPERTY(EditDefaultsOnly, Category = "Electricity")
	FLinearColor ElectricityCorruptColor;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void InitZone();

	/**
	 * @brief When zone is destroyed, the box changes size and overlapping actors are notified that they exit the weak zone
	 */
	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void DestroyZone();

	/**
	 * @brief When zone is created, the box changes size and overlapping actors are notified that they entered the weak zone
	 */
	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void CreateZone();

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void ChangeZoneSize(const FVector& NewSize);

#pragma region Cure

protected:
	UPROPERTY(EditDefaultsOnly, Category = "WeakZone|Cure")
	TObjectPtr<UCurveFloat> CureCurve;

	UPROPERTY(EditDefaultsOnly, Category = "WeakZone|Cure", meta = (ClampMin = 0.0f, Units = s))
	float CureDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category = "WeakZone|Cure")
	FName CureParam;

	FTimeline CureTimeline;

	UFUNCTION()
	void CureUpdate(float Alpha);

	UFUNCTION()
	void CureFinish();

	UPROPERTY(BlueprintAssignable, Category = "WeakZone")
	FOnCure OnCure;

	UPROPERTY(BlueprintAssignable, Category = "WeakZone")
	FOnCorrupt OnCorrupt;

	/**
	 * @brief Lights that will be shown when the zone is cured (will be hidden when corrupted)
	 */
	UPROPERTY(EditInstanceOnly, Category = "WeakZone")
	TArray<TObjectPtr<ALight>> CuredLights;

	/**
	 * @brief Lights that will be shown when the zone is corrupted (will be hidden when cured)
	 */
	UPROPERTY(EditInstanceOnly, Category = "WeakZone")
	TArray<TObjectPtr<ALight>> CorruptedLights;

	UPROPERTY(EditInstanceOnly, Category = "WeakZone")
	TArray<TObjectPtr<AActor>> CuredActors;

	UPROPERTY(EditInstanceOnly, Category = "WeakZone")
	TArray<TObjectPtr<AActor>> CorruptedActors;

#if WITH_EDITORONLY_DATA
	/**
	 * @brief Debug value to display the cured lights
	 */
	UPROPERTY(EditInstanceOnly, Transient, Category = "WeakZone")
	bool bShowWeakZoneAsCure = false;
#endif

	UPROPERTY(VisibleInstanceOnly, Category = "WeakZone")
	TArray<float> CuredLightsIntensity;

	UPROPERTY(VisibleInstanceOnly, Category = "WeakZone")
	TArray<float> CorruptedLightsIntensity;

	void SetActorsVisibility(bool bCure) const;

	void SetCuredActorsVisibility(bool bVisible) const;

	void SetCorruptedActorsVisibility(bool bVisible) const;

	void SetArrayVisibility(bool bVisible, const TArray<TObjectPtr<AActor>>& ActorArray) const;

public:
	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void CureZone(AActor* StartCurePoint);

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void CorruptZone(AActor* StartCorruptPoint);

#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeakZone")
	FVector ZoneSize = FVector(100.0f);

	UPROPERTY(EditDefaultsOnly, Category = "PostProcess")
	TObjectPtr<UMaterialInterface> ZoneMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "PostProcess")
	TObjectPtr<UMaterialInstanceDynamic> DynamicZoneMaterial;

	UPROPERTY(BlueprintReadOnly, Category = "PostProcess")
	FName RadiusParamName = "Radius";

	UPROPERTY(BlueprintReadOnly, Category = "PostProcess")
	FName LocationParamName = "Position";

	bool bIsZoneActive = true;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	void ActivateZone(bool bActivateZone);

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;

	virtual void FinishLoading(UENTWorldSave* LoadedWorldSave) override;

#pragma region Electricity

protected:
	UFUNCTION()
	void OnElectricityMovementFinished();
#pragma endregion 
};
