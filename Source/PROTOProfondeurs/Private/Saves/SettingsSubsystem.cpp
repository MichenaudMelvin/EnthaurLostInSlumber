// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/SettingsSubsystem.h"
#include "AkGameplayStatics.h"
#include "AkRtpc.h"
#include "Saves/SaveSettings.h"

USettingsSubsystem::USettingsSubsystem()
{
	SaveClass = USettingsSave::StaticClass();
}

UDefaultSave* USettingsSubsystem::CreateSave(const int SaveIndex)
{
	UDefaultSave* Save = Super::CreateSave(SaveIndex);

	if (!Save)
	{
		return nullptr;
	}

	USettingsSave* SettingsSave = Cast<USettingsSave>(Save);
	if (!SettingsSave)
	{
		return nullptr;
	}

	Settings = SettingsSave;

	return Settings;
}

void USettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadSave(0, true);
	Settings = Cast<USettingsSave>(SaveObject);

	SetMasterVolume(Settings->MasterVolume);
	SetMusicVolume(Settings->MusicVolume);
	SetSFXVolume(Settings->SFXVolume);
}

void USettingsSubsystem::SetMasterVolume(float Volume) const
{
	const USaveSettings* DevSettingsSave = GetDefault<USaveSettings>();
	if (!DevSettingsSave)
	{
		return;
	}

	UAkRtpc* RTPC = DevSettingsSave->MasterRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->MasterVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}

void USettingsSubsystem::SetSFXVolume(float Volume) const
{
	const USaveSettings* DevSettingsSave = GetDefault<USaveSettings>();
	if (!DevSettingsSave)
	{
		return;
	}

	UAkRtpc* RTPC = DevSettingsSave->SFXRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->SFXVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}

void USettingsSubsystem::SetMusicVolume(float Volume) const
{
	const USaveSettings* DevSettingsSave = GetDefault<USaveSettings>();
	if (!DevSettingsSave)
	{
		return;
	}

	UAkRtpc* RTPC = DevSettingsSave->MusicRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->MusicVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}
