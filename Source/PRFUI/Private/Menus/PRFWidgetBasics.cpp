// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/PRFWidgetBasics.h"

#include "PRFUIManager.h"
#include "Components/Button.h"

void UPRFWidgetBasics::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton && BackButton->GetCustomButton() && !BackButton->GetCustomButton()->OnClicked.IsAlreadyBound(this, &UPRFWidgetBasics::HandleBackAction))
	{
		BackButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFWidgetBasics::HandleBackAction);
	}
}

void UPRFWidgetBasics::BeginDestroy()
{
	Super::BeginDestroy();

	if (BackButton && BackButton->GetCustomButton())
	{
		BackButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFWidgetBasics::HandleBackAction);
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
