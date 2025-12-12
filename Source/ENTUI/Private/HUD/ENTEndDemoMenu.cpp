// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ENTEndDemoMenu.h"

#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTControllerMappingContext.h"
#include "Subsystems/ENTMenuManager.h"

void UENTEndDemoMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (InvisibleBackButton)
	{
		InvisibleBackButton->OnClicked.AddDynamic(this, &UENTEndDemoMenu::GoBackToMainMenu);
	}

	ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer();
	if (!IsValid(LocalPlayer))
	{
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!IsValid(InputSubsystem))
	{
		return;
	}

	APlayerController* Controller = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(Controller))
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(Controller, this, EMouseLockMode::DoNotLock, true);
	Controller->SetShowMouseCursor(false);
}

void UENTEndDemoMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (InvisibleBackButton)
	{
		InvisibleBackButton->OnClicked.RemoveDynamic(this, &UENTEndDemoMenu::GoBackToMainMenu);
	}
}

void UENTEndDemoMenu::GoBackToMainMenu()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(this, MainMenuLevel);
}
