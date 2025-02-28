// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NerveReceptacle.generated.h"

enum class ENerveReactiveInteractionType : uint8;
class INerveReactive;
class USphereComponent;

UCLASS()
class PROTOPROFONDEURS_API ANerveReceptacle : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANerveReceptacle();

protected:
	void EnableLinkedObjects();
	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void DisableLinkedObjects();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;

	UPROPERTY(EditAnywhere)
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;
};
