// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/ENTWidgetBasics.h"
#include "Subsystems/ENTMenuManager.h"
#include "Components/Button.h"
#include "Menus/Elements/ENTCustomButton.h"

void UENTWidgetBasics::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BackButton && BackButton->GetCustomButton() && !BackButton->GetCustomButton()->OnClicked.IsAlreadyBound(this, &UENTWidgetBasics::HandleBackAction))
	{
		BackButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTWidgetBasics::HandleBackAction);
	}
}

void UENTWidgetBasics::BeginDestroy()
{
	Super::BeginDestroy();

	if (BackButton && BackButton->GetCustomButton())
	{
		BackButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTWidgetBasics::HandleBackAction);
	}
}

void UENTWidgetBasics::HandleBackAction()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseCurrentMenu();
}
