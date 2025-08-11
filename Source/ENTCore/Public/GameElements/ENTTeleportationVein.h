// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTWeakZoneInterface.h"
#include "GameFramework/Actor.h"
#include "ENTTeleportationVein.generated.h"

UCLASS()
class ENTCORE_API AENTTeleportationVein : public AActor, public IENTWeakZoneInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AENTTeleportationVein();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<class UENTInteractableComponent> Interaction;

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
