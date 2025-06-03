// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmberOre.h"
#include "GameFramework/Actor.h"
#include "Saves/WorldSaves/SaveGameElementInterface.h"
#include "WeakZone.generated.h"

USTRUCT(BlueprintType)
struct FInteractionPoints
{
	GENERATED_BODY()

	UPROPERTY(VisibleInstanceOnly, Category = "InteractionsPoints")
	TObjectPtr<UStaticMeshComponent> MeshComp;

	UPROPERTY(VisibleInstanceOnly, Category = "InteractionsPoints")
	TObjectPtr<UStaticMeshComponent> AmberMeshComp;

	UPROPERTY(EditAnywhere, Category = "InteractionsPoints")
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InteractionsPoints", meta = (MakeEditWidget))
	FTransform Transform;

	bool bIsActive = false;
};

USTRUCT(BlueprintType)
struct FWeakZoneData : public FGameElementData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "WeakZone")
	TArray<bool> ActivatedInteractionPoints;
};

class UInteractableComponent;
class UBoxComponent;

UCLASS()
class PROTOPROFONDEURS_API AWeakZone : public AActor, public ISaveGameElementInterface
{
	GENERATED_BODY()

public:
	AWeakZone();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCure);

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

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
	TObjectPtr<UInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	TObjectPtr<UStaticMesh> DefaultInteractionPointMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	TObjectPtr<UStaticMesh> AmberMesh;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Interactable")
	TArray<FInteractionPoints> InteractionPoints;

	UPROPERTY(EditDefaultsOnly, Category = "Amber")
	EAmberType AmberType = EAmberType::WeakAmber;

	UPROPERTY(EditDefaultsOnly, Category = "Amber", meta = (ClampMin = 0))
	int CostByPoint = 1;

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
	virtual FGameElementData& SaveGameElement(UWorldSave* CurrentWorldSave) override;

	virtual void LoadGameElement(const FGameElementData& GameElementData) override;
};
