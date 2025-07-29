// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ENTHealthComponent.generated.h"

#pragma region Delegates

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKOnUpdateHealth, const float&, CurrentHealth, const float&, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FKOnReceiveDamages, const float&, CurrentHealth, const float&, MaxHealth);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FKOnHealthNull);

#pragma endregion

UCLASS(ClassGroup = "Health", meta = (BlueprintSpawnableComponent))
class ENTCOMPONENTS_API UENTHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UENTHealthComponent();

protected:
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

#pragma region Variables

protected:
	/**
	 * @brief The max health the object can have
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Health")
	float MaxHealth = 100.0f;

	/**
	 * @brief The current object health
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float CurrentHealth = 0.0f;

	/**
	 * @brief bool check to determine if the object can take damages or not
	 */
	UPROPERTY(BlueprintReadWrite, Category = "Health")
	bool bCanTakeDamages = true;

#pragma endregion

#pragma region HealthFunctions

public:
	/**
	 * @brief Set the current health of the object
	 * @param NewHealthValue The new health value
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealth(const float NewHealthValue);

	/**
	 * @brief Return the health in percent
	 * @return Health in percent
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercentage() const;

	/**
	 * @brief Return the health normalized
	 * @return Health normalized (between 0 and 1)
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetNormalizedHealth() const;

	/**
	 * @brief Set the max health of the object
	 * @param NewMaxHealth The new max health value
	 * @param bUpdateHealth Update the current health depending on the NewMaxHealth value
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetMaxHealth(const float NewMaxHealth = 100.0f, const bool bUpdateHealth = false);

	/**
	 * @brief Put the current health of the object to max health
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();
#pragma endregion

#pragma region DamagesFunctions
	/**
	 * @brief Broadcast when the health is updated
	 */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FKOnUpdateHealth OnUpdateHealth;

	/**
	 * @brief Broadcast when the object receive damages
	 */
	UPROPERTY(BlueprintAssignable, Category = "Damages")
	FKOnReceiveDamages OnReceiveDamages;

	/**
	 * @brief Broadcast when the object has no health
	 */
	UPROPERTY(BlueprintAssignable, Category = "Damages")
	FKOnHealthNull OnHealthNull;

	/**
	 * @brief Remove health to the object
	 * @param DamagesAmount the damages the object will take
	 */
	UFUNCTION(BlueprintCallable, Category = "Damages")
	void TakeDamages(const float DamagesAmount);

	/**
	 * @brief Remove health to the object
	 * @param Percent the damages the object will take in percent (total health)
	 */
	UFUNCTION(BlueprintCallable, Category = "Damages")
	void TakePercentDamages(const float Percent);

	/**
	 * @brief Kill the object
	 */
	UFUNCTION(BlueprintCallable, Category = "Damages")
	void TakeMaxDamages();
#pragma endregion

#pragma region InlineFunctions
	float GetMaxHealth() const {return MaxHealth;}

	float GetCurrentHealth() const {return CurrentHealth;}

	bool CanTakeDamages() const {return bCanTakeDamages;}

	void SetCanTakeDamages(const bool bValue) {bCanTakeDamages = bValue;}
#pragma endregion
};
