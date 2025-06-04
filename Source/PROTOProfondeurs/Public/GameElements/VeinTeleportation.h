// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "VeinTeleportation.generated.h"

UCLASS()
class PROTOPROFONDEURS_API AVeinTeleportation : public AActor, public IWeakZoneInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVeinTeleportation();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UInteractableComponent> Interaction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UWorld> MapToLoad;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnVeinInteracted);
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnVeinInteracted OnVeinInteracted;

#pragma region WeakZone

private:
	virtual void OnEnterWeakZone_Implementation(bool bIsZoneActive) override;

	virtual void OnExitWeakZone_Implementation() override;

	UFUNCTION()
	void OnInteract(APlayerController* Controller, APawn* Pawn, UPrimitiveComponent* InteractComponent);

#pragma endregion
};
