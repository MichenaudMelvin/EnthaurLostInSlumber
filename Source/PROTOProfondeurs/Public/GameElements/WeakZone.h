// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmberOre.h"
#include "GameFramework/Actor.h"
#include "WeakZone.generated.h"

class UInteractableComponent;
class UBoxComponent;

UCLASS()
class PROTOPROFONDEURS_API AWeakZone : public AActor
{
	GENERATED_BODY()

public:
	AWeakZone();

protected:
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Destroyed() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeakZone")
	TObjectPtr<UBoxComponent> BoxComponent;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UBillboardComponent> BillboardComponent;
#endif

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void InitZone();

	UFUNCTION(BlueprintCallable, Category = "WeakZone")
	void DestroyZone();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	FVector ZoneSize = FVector(100.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	FName ZoneLocationParamName = "ZoneLocation";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Materials")
	FName ZoneExtentParamName = "ZoneExtent";

	UPROPERTY(BlueprintReadOnly, Category = "Materials")
	TArray<TObjectPtr<UMaterialInstanceDynamic>> AllMaterialInstances;

	bool bIsZoneActive = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactable")
	TObjectPtr<UInteractableComponent> Interactable;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable")
	TObjectPtr<UStaticMesh> InteractionMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interactable")
	TArray<TObjectPtr<UStaticMeshComponent>> InteractionPoints;

	UPROPERTY(EditInstanceOnly, Category = "Interactable", meta = (DisplayName = "InteractionPoints", MakeEditWidget))
	TArray<FTransform> InteractionTransformPoints;

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
};
