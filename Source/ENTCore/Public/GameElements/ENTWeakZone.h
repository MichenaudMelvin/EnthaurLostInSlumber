// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTAmberOre.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTWeakZone.generated.h"

class UENTElectricityComponent;
class UAkAudioEvent;

class UENTInteractableComponent;
class UBoxComponent;
class UPostProcessComponent;

UCLASS()
class ENTCORE_API AENTWeakZone : public AActor, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AENTWeakZone();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeakZone")
	TObjectPtr<UPostProcessComponent> BlackAndWhiteShader;

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

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void DestroyZone();

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

#pragma region Electricity

protected:
	UFUNCTION()
	void OnElectricityMovementFinished();
#pragma endregion 
};
