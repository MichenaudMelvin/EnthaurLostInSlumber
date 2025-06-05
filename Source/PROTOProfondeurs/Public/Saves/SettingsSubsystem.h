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
	virtual UDefaultSave* CreateSave(const int SaveIndex) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<USettingsSave> Settings;

public:
	USettingsSave* GetSettings() const {return Settings;}

	void SetMasterVolume(float Volume) const;

	void SetSFXVolume(float Volume) const;

	void SetMusicVolume(float Volume) const;
};
