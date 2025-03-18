// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveSubsystem.h"
#include "SettingsSave.h"
#include "SettingsSubsystem.generated.h"

UCLASS()
class PROTOPROFONDEURS_API USettingsSubsystem : public USaveSubsystem
{
	GENERATED_BODY()

public:
	USettingsSubsystem();

protected:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USettingsSave> Settings;

public:
	USettingsSave* GetSettings() const {return Settings;}
};
