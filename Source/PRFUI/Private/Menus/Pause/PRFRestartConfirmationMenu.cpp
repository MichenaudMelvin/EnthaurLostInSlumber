// Fill out your copyright notice in the Description page of Project Settings.


#include "Menus/Pause/PRFRestartConfirmationMenu.h"

#include "PRFUIManager.h"
#include "GameElements/RespawnTree.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UPRFRestartConfirmationMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.AddDynamic(this, &UPRFRestartConfirmationMenu::HandleRestartAction);
	}
}

void UPRFRestartConfirmationMenu::BeginDestroy()
{
	Super::BeginDestroy();

	if (RestartButton && RestartButton->GetCustomButton())
	{
		RestartButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UPRFRestartConfirmationMenu::HandleRestartAction);
	}
}

void UPRFRestartConfirmationMenu::HandleRestartAction()
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
