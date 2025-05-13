// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/PRFWidgetBasics.h"

#include "PRFUIManager.h"
#include "Components/Button.h"

void UPRFWidgetBasics::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton)
	{
		BackButton->OnClicked.AddDynamic(this, &UPRFWidgetBasics::HandleBackAction);
	}
}

void UPRFWidgetBasics::BeginDestroy()
{
	Super::BeginDestroy();

	if (BackButton)
	{
		BackButton->OnClicked.RemoveDynamic(this, &UPRFWidgetBasics::HandleBackAction);
	}
}

void UPRFWidgetBasics::HandleBackAction()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseCurrentMenu();
}
