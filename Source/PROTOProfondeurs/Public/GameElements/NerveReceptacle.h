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
	ANerveReceptacle();
	void TriggerLinkedObjects();

	// Create Event
	UFUNCTION(BlueprintImplementableEvent, Category = "BaseCharacter")
	void OnNerveConnect();

protected:
	UFUNCTION()
	void TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TObjectPtr<USphereComponent> Collision;

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> NerveReceptacle;

	UPROPERTY(EditAnywhere)
	TMap<AActor*, ENerveReactiveInteractionType> ObjectReactive;

	UPROPERTY(EditAnywhere)
	FName ConnectedShaderTag;
	
};
