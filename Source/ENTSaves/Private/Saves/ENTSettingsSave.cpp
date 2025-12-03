// Fill out your copyright notice in the Description page of Project Settings.


#include "Saves/ENTSettingsSave.h"

UENTSettingsSave::UENTSettingsSave()
{
	SlotName = "SettingsSave";
}

void UENTSettingsSave::ResetVolumeSettings()
{
	MasterVolume = 80.0f;
	MusicVolume = 80.0f;
	SFXVolume = 80.0f;
}

void UENTSettingsSave::ResetAccessibilitySettings()
{
	MouseSensitivity = 1.0f;
	bInvertYAxis = false;
	bViewBobbing = true;
}

void UENTSettingsSave::ResetBrightnessSettings()
{
	Gamma = 1.0f;
}
