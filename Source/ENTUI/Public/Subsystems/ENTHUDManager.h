// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ENTHUDManager.generated.h"

class UENTLevelEntering;
class UENTDeathTransition;
class UENTGameplayHUD;
class UENTPropulsionConstraint;

UCLASS()
class ENTUI_API UENTHUDManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

#pragma region Delegates

public:
	FDelegateHandle CreateHUDWidgetsDelegate;

	FDelegateHandle WorldInitActorsDelegateHandle;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHUDWidgetsCreated);

	UPROPERTY(BlueprintAssignable)
	FOnHUDWidgetsCreated OnHUDWidgetsCreated;

#pragma endregion

#pragma region Widgets

protected:
	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<UENTGameplayHUD> GameplayHUD;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<UENTDeathTransition> DeathTransition;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TObjectPtr<UENTLevelEntering> LevelEntering;

	UPROPERTY(BlueprintReadOnly, Category = "UI|HUD")
	TArray<UUserWidget*> AllHUDWidgets;

	void CreateHUDWidgets();

public:
	TObjectPtr<UENTGameplayHUD> GetGameplayHUD() const {return GameplayHUD;}

	TObjectPtr<UENTDeathTransition> GetDeathTransition() const {return DeathTransition;}

	TObjectPtr<UENTLevelEntering> GetLevelEntering() {return LevelEntering;}

#pragma endregion

#pragma region HUD

protected:
	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void DisplayHUD();

	UFUNCTION()
	void DisplayDeathTransition();

	UFUNCTION(BlueprintCallable, Category = "UI|HUD")
	void HideHUD();

public:
	void SetHUDVisibility(const ESlateVisibility& Visibility);

#pragma endregion

#pragma region Constraint

protected:
	UFUNCTION()
	void BindConstraintDelegates(UENTPropulsionConstraint* Constraint);

	UPROPERTY()
	TObjectPtr<UENTPropulsionConstraint> CurrentConstraint;

#pragma endregion

protected:
	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);
};
