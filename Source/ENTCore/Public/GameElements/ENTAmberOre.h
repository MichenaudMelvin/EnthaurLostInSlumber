// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "ENTAmberOre.generated.h"

enum class ENerveReactiveInteractionType : uint8;
class UAkAudioEvent;
class AENTWeakZone;
class UAkComponent;
class UBoxComponent;

class UENTInteractableComponent;

UENUM()
enum class EAmberType : uint8
{
	NecroseAmber,
	WeakAmber
};

UCLASS()
class ENTCORE_API AENTAmberOre : public AActor, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AENTAmberOre();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Amber")
	TObjectPtr<UStaticMeshComponent> AmberMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> MeshInteraction;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UENTInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UAkComponent> AmberOreNoises;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UStaticMesh> SourceMesh;

	UPROPERTY()
	EAmberType AmberType = EAmberType::WeakAmber;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	float EmptyAmberHeight = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	float FullAmberHeight = 35.0f;

	UPROPERTY()
	float TargetAmberHeight;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Amber")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactiveFull;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Amber")
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactiveEmpty;

	UPROPERTY(EditDefaultsOnly, Category = "Weak Zone")
	TObjectPtr<UAkAudioEvent> GrowlNoise;

	UPROPERTY(EditDefaultsOnly, Category = "Weak Zone")
	TObjectPtr<UAkAudioEvent> InjectAmberNoise;

	UPROPERTY(EditInstanceOnly, Category = "Amber")
	bool bIsEmpty;

	UPROPERTY(EditInstanceOnly, Category = "Weak Zone")
	TObjectPtr<AENTWeakZone> LinkedWeakZone;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	TObjectPtr<UCurveFloat> FillAmberCurve;

	UPROPERTY(EditDefaultsOnly, Category = "Amber", meta = (Units = s))
	float FillAmberDuration = 1.5f;

	FTimeline FillAmberTimeline;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractionComponent);

	UFUNCTION()
	void FillAmberUpdate(float Alpha);

	UFUNCTION()
	void FillAmberFinished();

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

	void TriggerLinkedObjects(TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive);

	void TriggerFullLinkedObjects();
	
	void TriggerEmptyLinkedObjects();


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

	virtual void LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave) override;
};
