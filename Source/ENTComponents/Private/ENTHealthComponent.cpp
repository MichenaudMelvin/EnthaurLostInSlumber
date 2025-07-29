// Fill out your copyright notice in the Description page of Project Settings.

#include "ENTHealthComponent.h"
#include "Kismet/KismetMathLibrary.h"

UENTHealthComponent::UENTHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UENTHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	OnUpdateHealth.Broadcast(CurrentHealth, MaxHealth);
}

void UENTHealthComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if (!GetOwner())
	{
		return;
	}

	OnUpdateHealth.RemoveAll(GetOwner());
	OnReceiveDamages.RemoveAll(GetOwner());
	OnHealthNull.RemoveAll(GetOwner());
}

void UENTHealthComponent::TakeDamages(const float DamagesAmount)
{
	if(!bCanTakeDamages)
	{
		return;
	}

	CurrentHealth = FMath::Clamp((CurrentHealth - DamagesAmount), 0.0f, MaxHealth);

	OnUpdateHealth.Broadcast(CurrentHealth, MaxHealth);
	OnReceiveDamages.Broadcast(CurrentHealth, MaxHealth);

	if(CurrentHealth != 0)
	{
		return;
	}

	OnHealthNull.Broadcast();
}

void UENTHealthComponent::TakePercentDamages(const float Percent)
{
	TakeDamages(Percent * MaxHealth / 100);
}

void UENTHealthComponent::TakeMaxDamages()
{
	TakeDamages(CurrentHealth);
}

void UENTHealthComponent::SetHealth(const float NewHealthValue)
{
	CurrentHealth = FMath::Clamp(NewHealthValue, 0.0f, MaxHealth);
	OnUpdateHealth.Broadcast(CurrentHealth, MaxHealth);

	if(CurrentHealth > 0)
	{
		return;
	}

	OnHealthNull.Broadcast();
}

float UENTHealthComponent::GetHealthPercentage() const
{
	return GetCurrentHealth() * 100 / GetMaxHealth();
}

float UENTHealthComponent::GetNormalizedHealth() const
{
	return UKismetMathLibrary::NormalizeToRange(CurrentHealth, 0, GetMaxHealth());
}

void UENTHealthComponent::SetMaxHealth(const float NewMaxHealth, const bool bUpdateHealth)
{
	const float OldMaxHealth = MaxHealth;
	MaxHealth = NewMaxHealth;

	if (bUpdateHealth)
	{
		CurrentHealth += NewMaxHealth - OldMaxHealth;
	}

	OnUpdateHealth.Broadcast(CurrentHealth, MaxHealth);
}

void UENTHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;

	OnUpdateHealth.Broadcast(CurrentHealth, MaxHealth);
}
