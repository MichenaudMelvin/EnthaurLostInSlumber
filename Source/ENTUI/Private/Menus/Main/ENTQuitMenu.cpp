// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/ENTQuitMenu.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Menus/Elements/ENTCustomButton.h"

void UENTQuitMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (QuitGameButton && QuitGameButton->GetCustomButton())
	{
		QuitGameButton->GetCustomButton()->OnClicked.AddDynamic(this, &UENTQuitMenu::HandleQuitGameInteraction);
	}
}

void UENTQuitMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (QuitGameButton && QuitGameButton->GetCustomButton())
	{
		QuitGameButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UENTQuitMenu::HandleQuitGameInteraction);
	}
}

void UENTQuitMenu::HandleQuitGameInteraction()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController))
	{
		return;
	}

	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}
