// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TextBlock.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTSoundMenu.generated.h"

class UENTCustomSlider;
/**
 * 
 */
UCLASS()
class ENTUI_API UENTSoundMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;

protected:
	UFUNCTION()
	void OnOverallSliderChanged(float InValue);

	UFUNCTION()
	void OnOverallButtonHovered();

	UFUNCTION()
	void OnMusicSliderChanged(float InValue);

	UFUNCTION()
	void OnMusicButtonHovered();

	UFUNCTION()
	void OnSfxSliderChanged(float InValue);

	UFUNCTION()
	void OnSfxButtonHovered();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> OverallVolumeButton;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> OverallVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OverallVolumeValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> MusicVolumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> MusicVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> MusicVolumeValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomButton> SfxVolumeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UENTCustomSlider> SfxVolumeSlider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> SfxVolumeValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionTitle;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> OptionDescription;
};
