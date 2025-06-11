// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DeathMenuUI.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "GameElements/RespawnTree.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"

void UDeathMenuUI::NativeConstruct()
{
	Super::NativeConstruct();

	if (RespawnButton && RespawnButton->GetCustomButton())
	{
		RespawnButton->GetCustomButton()->OnClicked.AddDynamic(this, &UDeathMenuUI::RespawnPlayer);
	}
	if (BackMainMenuButton && BackMainMenuButton->GetCustomButton())
	{
		BackMainMenuButton->GetCustomButton()->OnClicked.AddDynamic(this, &UDeathMenuUI::BackToMainMenu);
	}
}

void UDeathMenuUI::NativeDestruct()
{
	Super::NativeDestruct();

	if (RespawnButton && RespawnButton->GetCustomButton())
	{
		RespawnButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UDeathMenuUI::RespawnPlayer);
	}
	if (BackMainMenuButton && BackMainMenuButton->GetCustomButton())
	{
		BackMainMenuButton->GetCustomButton()->OnClicked.RemoveDynamic(this, &UDeathMenuUI::BackToMainMenu);
	}
}

void UDeathMenuUI::RespawnPlayer()
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

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController)
	{
		return;
	}

	PlayerController->SetPause(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController, true);
	PlayerController->SetShowMouseCursor(false);
}

void UDeathMenuUI::BackToMainMenu()
{
	UGameplayStatics::OpenLevelBySoftObjectPtr(
		this,
		MainMenuLevel);
}
