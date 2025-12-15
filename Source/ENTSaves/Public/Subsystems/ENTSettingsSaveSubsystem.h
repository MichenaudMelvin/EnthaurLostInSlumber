// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTSaveSubsystem.h"
#include "ENTSettingsSaveSubsystem.generated.h"

class UENTSettingsSave;
class UCameraComponent;

UCLASS()
class ENTSAVES_API UENTSettingsSaveSubsystem : public UENTSaveSubsystem
{
	GENERATED_BODY()

public:
	UENTSettingsSaveSubsystem();

	virtual void ResetSaveToDefault(const int SaveIndex) override;

protected:
	virtual UENTDefaultSave* CreateSave(const int SaveIndex) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	UPROPERTY(BlueprintReadOnly, Category = "Settings")
	TObjectPtr<UENTSettingsSave> Settings;

	UPROPERTY()
	TObjectPtr<UCameraComponent> CurrentCamera;

	FDelegateHandle WorldInitDelegateHandle;

	FDelegateHandle WorldBeginPlayDelegateHandle;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);
	void OnNewWorldBeginPlay();

public:
	UENTSettingsSave* GetSettings() const {return Settings;}

	void SetGamma(float Gamma) const;

	void SetMasterVolume(float Volume) const;

	void SetSFXVolume(float Volume) const;

	void SetMusicVolume(float Volume) const;
};
