// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTAmberOre.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTWeakZone.generated.h"

class UAkAudioEvent;

USTRUCT(BlueprintType)
struct FENTInteractionPoints
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, Category = "InteractionsPoints")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleInstanceOnly, Category = "InteractionsPoints")
	TObjectPtr<UStaticMeshComponent> AmberMeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InteractionsPoints")
	TObjectPtr<UBoxComponent> InteractionBox;

	UPROPERTY(EditAnywhere, Category = "InteractionsPoints")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionsPoints", meta = (MakeEditWidget))
	FTransform Transform;

	bool bIsActive = false;
};

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

	UPROPERTY(BlueprintReadWrite, Category = "WeakZone")
	TObjectPtr<UMaterialInstanceDynamic> MaterialBlackAndWhite;

	UPROPERTY(EditDefaultsOnly, Category = "WeakZone")
	TObjectPtr<UAkAudioEvent> GrowlNoise;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void InitZone();

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void DestroyZone();

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

#pragma endregion

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WeakZone")
	FVector ZoneSize = FVector(100.0f);

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> DynamicPPMaterial;

	bool bIsZoneActive = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactable")
	TObjectPtr<UENTInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	TObjectPtr<UStaticMesh> DefaultInteractionPointMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	TObjectPtr<UStaticMesh> AmberMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	float TargetAmberHeight = 35.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	float AmberAnimSpeed = 1.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	FVector InteractionBoxExtent = FVector(100.0f, 100.0f, 30.0f);

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interactable")
	TArray<FENTInteractionPoints> InteractionPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	EAmberType AmberType = EAmberType::WeakAmber;

	UPROPERTY(EditDefaultsOnly, Category = "Amber", meta = (ClampMin = 0))
	int CostByPoint = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UAkAudioEvent> InjectAmberNoise;

	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	FENTInteractionPoints* FindInteractionPoint(TObjectPtr<UPrimitiveComponent> StaticMeshComponent);

	bool IsEveryInteractionPointsActive() const;

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void CheckIfEveryInteractionsPointActive();

#pragma region Foliage

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Foliage")
	TObjectPtr<UInstancedStaticMeshComponent> Foliage;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	TObjectPtr<UStaticMesh> FoliageMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	FVector FoliageScale = FVector(0.1f);

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	FFloatRange FoliageOffsetRange = FFloatRange(250.0f, 500.0f);

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	uint16 MeshesNumberByInteractionsPoints = 75;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage|Trace")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectsTypes;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage|Trace", meta = (Units = cm))
	float TraceLength = 100.0f;

	UPROPERTY(EditInstanceOnly, Category = "Foliage")
	FRandomStream Seed;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	TObjectPtr<UCurveFloat> FoliageGrowthCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage")
	TObjectPtr<UAkAudioEvent> FoliageGrowthNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Foliage", meta = (Units = s))
	float GrowthDuration = 3.0f;

	FTimeline FoliageTimeline;

	UFUNCTION()
	void FoliageGrowthUpdate(float Alpha);

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Transient, Category = "Foliage|Debug")
	bool bShowFoliage = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Foliage|Debug")
	bool bShowTraces = false;

	UPROPERTY(EditInstanceOnly, Transient, Category = "Foliage|Debug")
	float TracesSize = 15.0f;
#endif

#pragma endregion

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData) override;
};
