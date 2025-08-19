// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TimelineComponent.h"
#include "ENTLevelEntering.generated.h"

struct FENTZoneName;
class UCanvasPanelSlot;
class UHorizontalBox;
class UImage;
class UTextBlock;

UCLASS()
class ENTUI_API UENTLevelEntering : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;

#if WITH_EDITOR
	virtual void NativePreConstruct() override;
#endif

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UTextBlock> AreaName;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UTextBlock> RegionName;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UPanelWidget> AreaContainer;

	UPROPERTY(BlueprintReadWrite, Category = "Text", meta = (BindWidget))
	TObjectPtr<UPanelWidget> RegionContainer;

	UPROPERTY(BlueprintReadOnly, Category = "Animations", Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> DisplayLevelName;

public:
	void SetZoneName(const FENTZoneName& ZoneName) const;

#pragma region Underlines

	UPROPERTY(BlueprintReadOnly, Category = "Underlines", meta = (BindWidget))
	TObjectPtr<UImage> LeftUnderline;

	UPROPERTY(BlueprintReadOnly, Category = "Underlines", meta = (BindWidget))
	TObjectPtr<UImage> RightUnderline;

	UPROPERTY(EditDefaultsOnly, Category = "Underlines")
	float RegionOffset = 50.0f;

	float UnderlineStartPoint = 0.0f;

	float UnderlineEndPoint = 0.0f;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> LeftUnderlineSlot;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> RightUnderlineSlot;

	UPROPERTY(EditDefaultsOnly, Category = "Underlines")
	TObjectPtr<UCurveFloat> UnderlinesCurve;

	FTimeline UnderlinesTimeline;

	/**
	 * @brief Will auto set underline start and end points
	 * @param LeftMargin 
	 * @param RightMargin 
	 */
	void GetStartMargins(FMargin& LeftMargin, FMargin& RightMargin);

	UFUNCTION(BlueprintCallable, Category = "Underlines")
	void StartUnderlinesLerp();

	UFUNCTION()
	void UnderlinesUpdate(float Alpha);

#pragma endregion
};
