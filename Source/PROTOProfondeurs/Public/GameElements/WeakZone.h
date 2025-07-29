// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmberOre.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/ENTSaveGameElementInterface.h"
#include "WeakZone.generated.h"

class UAkAudioEvent;

USTRUCT(BlueprintType)
struct FInteractionPoints
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

UCLASS()
class PROTOPROFONDEURS_API AWeakZone : public AActor, public IENTSaveGameElementInterface
{
	GENERATED_BODY()

public:
	AWeakZone();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCure);

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WeakZone")
	TObjectPtr<class UPostProcessComponent> BlackAndWhiteShader;

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UMaterialInstanceDynamic> MaterialBlackAndWhite;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnCure OnCure;

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

	UPROPERTY(EditDefaultsOnly, Category = "WeakZone", meta = (ClampMin = 0.0f, Units = s))
	float CureDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
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
	TArray<FInteractionPoints> InteractionPoints;

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

	FInteractionPoints* FindInteractionPoint(TObjectPtr<UPrimitiveComponent> StaticMeshComponent);

	bool IsEveryInteractionPointsActive() const;

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void CheckIfEveryInteractionsPointActive();

public:
	virtual FENTGameElementData& SaveGameElement(UENTWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FENTGameElementData& GameElementData) override;
};
