// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Main/PRFQuitMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UPRFQuitMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (QuitGameButton && QuitGameButton->GetCustomButton())
	{
		QuitGameButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFQuitMenu::HandleQuitGameInteraction);
	}
}

void UPRFQuitMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (QuitGameButton && QuitGameButton->GetCustomButton())
	{
		QuitGameButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFQuitMenu::HandleQuitGameInteraction);
	}
}

void UPRFQuitMenu::HandleQuitGameInteraction()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	UKismetSystemLibrary::QuitGame(this, PlayerController, EQuitPreference::Quit, false);
}
