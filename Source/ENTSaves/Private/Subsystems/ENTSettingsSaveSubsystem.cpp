// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTSettingsSaveSubsystem.h"
#include "Saves/ENTSettingsSave.h"
#include "AkGameplayStatics.h"
#include "AkRtpc.h"
#include "Camera/CameraComponent.h"
#include "Config/ENTSavesConfig.h"

UENTSettingsSaveSubsystem::UENTSettingsSaveSubsystem()
{
	SaveClass = UENTSettingsSave::StaticClass();
}

UENTDefaultSave* UENTSettingsSaveSubsystem::CreateSave(const int SaveIndex)
{
	UENTDefaultSave* Save = Super::CreateSave(SaveIndex);

	if (!Save)
	{
		return nullptr;
	}

	UENTSettingsSave* SettingsSave = Cast<UENTSettingsSave>(Save);
	if (!SettingsSave)
	{
		return nullptr;
	}

	Settings = SettingsSave;

	return Settings;
}

void UENTSettingsSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	WorldInitDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UENTSettingsSaveSubsystem::OnNewWorldStarted);

	LoadSave(0, true);
	Settings = Cast<UENTSettingsSave>(SaveObject);

	SetMasterVolume(Settings->MasterVolume);
	SetMusicVolume(Settings->MusicVolume);
	SetSFXVolume(Settings->SFXVolume);
}

void UENTSettingsSaveSubsystem::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnWorldInitializedActors.Remove(WorldInitDelegateHandle);
}

void UENTSettingsSaveSubsystem::ResetSaveToDefault(const int SaveIndex)
{
	Super::ResetSaveToDefault(SaveIndex);

	if (!Settings)
	{
		return;
	}

	SetGamma(Settings->Gamma);
}

void UENTSettingsSaveSubsystem::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
{
	if (GetWorld())
	{
		WorldBeginPlayDelegateHandle = GetWorld()->OnWorldBeginPlay.AddUObject(this, &UENTSettingsSaveSubsystem::OnNewWorldBeginPlay);
	}
}

void UENTSettingsSaveSubsystem::OnNewWorldBeginPlay()
{
	GetWorld()->OnWorldBeginPlay.Remove(WorldBeginPlayDelegateHandle);

	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!Pawn)
	{
		return;
	}

	UCameraComponent* CameraComponent = Pawn->GetComponentByClass<UCameraComponent>();
	CurrentCamera = CameraComponent;
	SetGamma(Settings->Gamma);
}

void UENTSettingsSaveSubsystem::SetGamma(float Gamma) const
{
	Settings->Gamma = Gamma;

	if (CurrentCamera)
	{
		CurrentCamera->PostProcessSettings.bOverride_ColorGamma = true;
		CurrentCamera->PostProcessSettings.ColorGamma = FVector4(Settings->Gamma, Settings->Gamma, Settings->Gamma);
	}
}

void UENTSettingsSaveSubsystem::SetMasterVolume(float Volume) const
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return;
	}

	UAkRtpc* RTPC = Config->MasterRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->MasterVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}

void UENTSettingsSaveSubsystem::SetSFXVolume(float Volume) const
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return;
	}

	UAkRtpc* RTPC = Config->SFXRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->SFXVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}

void UENTSettingsSaveSubsystem::SetMusicVolume(float Volume) const
{
	const UENTSavesConfig* Config = GetDefault<UENTSavesConfig>();
	if (!Config)
	{
		return;
	}

	UAkRtpc* RTPC = Config->MusicRTPC.LoadSynchronous();
	if(!RTPC)
	{
		return;
	}

	Settings->MusicVolume = Volume;
	UAkGameplayStatics::SetRTPCValue(RTPC, Volume, 0, nullptr);
}
