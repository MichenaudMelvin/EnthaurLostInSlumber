// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFPauseMenu.h"
#include "PRFUIManager.h"
#include "UIManagerSettings.h"
#include "GameElements/RespawnTree.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"
#include "Subsystems/LevelNameSubsystem.h"

void UPRFPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (OptionsMenuButton && OptionsMenuButton->GetCustomButton())
	{
		OptionsMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton && RestartCheckpointButton->GetCustomButton())
	{
		RestartCheckpointButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFPauseMenu::HandleQuitButton);
	}

	
	ULevelNameSubsystem* LevelNameSubsystem = GetGameInstance()->GetSubsystem<ULevelNameSubsystem>();
	if (!IsValid(LevelNameSubsystem))
	{
		return;
	}

	AreaName->SetText(LevelNameSubsystem->GetLevelName().AreaZone);
	RegionName->SetText(LevelNameSubsystem->GetLevelName().RegionZone);
}

void UPRFPauseMenu::NativeDestruct()
{
	Super::NativeDestruct();

	if (OptionsMenuButton && OptionsMenuButton->GetCustomButton())
	{
		OptionsMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleOptionsMenuButton);
	}
	if (RestartCheckpointButton && RestartCheckpointButton->GetCustomButton())
	{
		RestartCheckpointButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleRestartCheckpointButton);
	}
	if (MainMenuButton && MainMenuButton->GetCustomButton())
	{
		MainMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleMainMenuButton);
	}
	if (QuitButton && QuitButton->GetCustomButton())
	{
		QuitButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFPauseMenu::HandleQuitButton);
	}
}

void UPRFPauseMenu::HandleMainMenuButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* MainMenuConfirmationMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->MainMenuConfirmationMenuClass);
	UIManager->OpenMenu(MainMenuConfirmationMenu, false);
}

void UPRFPauseMenu::HandleQuitButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	UUserWidget* QuitMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->QuitMenuClass);
	UIManager->OpenMenu(QuitMenu, false);
}

void UPRFPauseMenu::HandleOptionsMenuButton()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	const UUIManagerSettings* UIManagerSettings = GetDefault<UUIManagerSettings>();
	if (!IsValid(UIManagerSettings))
	{
		return;
	}

	if (!IsValid(UIManagerSettings->OptionsMenuClass))
	{
		return;
	}

	UUserWidget* OptionsMenu = CreateWidget<UUserWidget>(GetWorld(), UIManagerSettings->OptionsMenuClass);
	UIManager->OpenMenu(OptionsMenu, false);
}

void UPRFPauseMenu::HandleRestartCheckpointButton()
{
	ACharacter* Character = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Character)
	{
		return;
	}

	AFirstPersonCharacter* Player = Cast<AFirstPersonCharacter>(Character);
	if (!Player)
	{
		return;
	}

	ARespawnTree* RespawnTree = Player->GetRespawnTree();
	FTransform RespawnTransform(FTransform::Identity);
	if (RespawnTree)
	{
		RespawnTransform = RespawnTree->GetRespawnTransform();
	}
	else
	{
		AActor* PlayerStart = UGameplayStatics::GetActorOfClass(this, APlayerStart::StaticClass());
		if (PlayerStart)
		{
			RespawnTransform = PlayerStart->GetActorTransform();
		}

#if WITH_EDITOR
		else
		{
			const FString Message = FString::Printf(TEXT("Missing playerStart in this level"));

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		}
#endif
	}

	Player->Respawn(RespawnTransform);

	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus(EPRFUIState::Gameplay);
}
