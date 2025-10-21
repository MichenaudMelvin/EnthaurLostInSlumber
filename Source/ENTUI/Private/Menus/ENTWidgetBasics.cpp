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

	// if (FadeAnimation)
	// {
	// 	OnFadeAnimationFinishedHandle.AddDynamic(this,&UENTWidgetBasics::RemoveMenuAfterAnim);
	// }
}

void UENTWidgetBasics::NativeConstruct()
{
	Super::NativeConstruct();

	if (FadeAnimation)
	{
		PlayFadeAnimation(false);
	}
}

void UENTWidgetBasics::BeginDestroy()
{
	Super::BeginDestroy();

	if (BackButton && BackButton->GetCustomButton())
	{
		BackButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTWidgetBasics::HandleBackAction);
	}
	// if (FadeAnimation)
	// {
	// 	OnFadeAnimationFinishedHandle.RemoveDynamic(this, &UENTWidgetBasics::RemoveMenuAfterAnim);
	// }
}

void UENTWidgetBasics::PlayFadeAnimation(bool bIsReverse)
{
	if (!FadeAnimation)
	{
		RemoveFromParent();
		return;
	}

	FWidgetAnimationDynamicEvent FinishedAnimationEventHandle;
	FinishedAnimationEventHandle.BindDynamic(this,&UENTWidgetBasics::RemoveMenuAfterAnim);
	BindToAnimationFinished(FadeAnimation, FinishedAnimationEventHandle);
	
	PlayParams.StartAtTime = 0.0f;
	PlayParams.NumLoopsToPlay = 1;
	bIsReverse ? PlayParams.PlayMode = EUMGSequencePlayMode::Reverse : PlayParams.PlayMode = EUMGSequencePlayMode::Forward;
	PlayParams.PlaybackSpeed = 1.0f;
	PlayParams.bRestoreState = false;
	
	PlayAnimation(FadeAnimation, PlayParams.StartAtTime, PlayParams.NumLoopsToPlay, PlayParams.PlayMode, PlayParams.PlaybackSpeed, PlayParams.bRestoreState);
}

void UENTWidgetBasics::RemoveMenuAfterAnim()
{
	OnFadeAnimationFinishedHandle.Broadcast(this);
	
	if (PlayParams.PlayMode == EUMGSequencePlayMode::Reverse)
	{
		RemoveFromParent();
	}
}


void UENTWidgetBasics::HandleBackAction()
{
	UENTMenuManager* MenuManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(MenuManager))
	{
		return;
	}

	MenuManager->CloseCurrentMenu();
}
