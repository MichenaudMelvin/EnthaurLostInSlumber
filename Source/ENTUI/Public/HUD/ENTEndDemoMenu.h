// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Menus/ENTWidgetBasics.h"
#include "ENTEndDemoMenu.generated.h"

/**
 * 
 */
UCLASS()
class ENTUI_API UENTEndDemoMenu : public UENTWidgetBasics
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
protected:
	UFUNCTION()
	void GoBackToMainMenu();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> GameLogo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> ExitText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> InvisibleBackButton;

	UPROPERTY(BlueprintReadwrite)
	TSoftObjectPtr<ULevel> MainMenuLevel;
};
