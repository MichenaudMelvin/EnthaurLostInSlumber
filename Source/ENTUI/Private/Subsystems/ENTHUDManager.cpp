// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/ENTHUDManager.h"
#include "Config/ENTUIConfig.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EditorSettings/ENTEditorSettings.h"
#include "ENTComponents/Public/ENTHealthComponent.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ENTDefaultCharacter.h"

void UENTHUDManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// OnWorldBeginPlay isn't called properly in packaged games at Initialize()
	WorldInitActorsDelegateHandle = FWorldDelegates::OnWorldInitializedActors.AddUObject(this, &UENTHUDManager::OnNewWorldStarted);
}

void UENTHUDManager::Deinitialize()
{
	Super::Deinitialize();

	FWorldDelegates::OnPostWorldInitialization.Remove(WorldInitActorsDelegateHandle);
}

void UENTHUDManager::CreateHUDWidgets()
{
	GetWorld()->OnWorldBeginPlay.Remove(CreateHUDWidgetsDelegate);

	const UENTUIConfig* UIConfig = GetDefault<UENTUIConfig>();
	if (!IsValid(UIConfig))
	{
		return;
	}

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	GameplayHUD = CreateWidget<UENTGameplayHUD>(PlayerController, UIConfig->GameplayHUDClass);
	DeathTransition = CreateWidget<UENTDeathTransition>(PlayerController, UIConfig->DeathTransitionClass);
	LevelEntering = CreateWidget<UENTLevelEntering>(PlayerController, UIConfig->LevelEnteringClass);

	AllHUDWidgets.Add(GameplayHUD);
	AllHUDWidgets.Add(DeathTransition);
	AllHUDWidgets.Add(LevelEntering);

	OnHUDWidgetsCreated.Broadcast();

	APawn* Pawn = PlayerController->GetPawn();
	if (!Pawn)
	{
		return;
	}

	AENTDefaultCharacter* Player = Cast<AENTDefaultCharacter>(Pawn);
	if (!Player)
	{
		return;
	}

	UENTHealthComponent* HealthComponent = Player->GetHealth();
	if (HealthComponent)
	{
		HealthComponent->OnHealthNull.AddDynamic(this, &UENTHUDManager::DisplayDeathTransition);
	}

	Player->OnConstraintAdded.AddDynamic(this, &UENTHUDManager::BindConstraintDelegates);
	Player->OnInteractionFeedback.AddDynamic(GameplayHUD, &UENTGameplayHUD::SetInteraction);
}

void UENTHUDManager::DisplayHUD()
{
	if (AllHUDWidgets.Num() == 0)
	{
		if (!OnHUDWidgetsCreated.IsAlreadyBound(this, &UENTHUDManager::DisplayHUD))
		{
			OnHUDWidgetsCreated.AddDynamic(this, &UENTHUDManager::DisplayHUD);
		}

		return;
	}

	if (OnHUDWidgetsCreated.IsAlreadyBound(this, &UENTHUDManager::DisplayHUD))
	{
		OnHUDWidgetsCreated.RemoveDynamic(this, &UENTHUDManager::DisplayHUD);
	}

	if (GameplayHUD)
	{
		GameplayHUD->AddToViewport();
	}

	if(!LevelEntering)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	const UENTEditorSettings* EditorSettings = GetDefault<UENTEditorSettings>();

	if (!EditorSettings || !EditorSettings->bDisplayStartWidget)
	{
		return;
	}
#endif

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
	{
		return;
	}

	UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(PlayerController, LevelEntering);

	LevelEntering->AddToViewport(4);
}

void UENTHUDManager::DisplayDeathTransition()
{
	DeathTransition->AddToViewport();
}

void UENTHUDManager::HideHUD()
{
	GameplayHUD->RemoveFromParent();
	DeathTransition->RemoveFromParent();
	LevelEntering->RemoveFromParent();
}

void UENTHUDManager::SetHUDVisibility(const ESlateVisibility& Visibility)
{
	for (UUserWidget* HUDWidget : AllHUDWidgets)
	{
		if (!HUDWidget)
		{
			continue;
		}

		HUDWidget->SetVisibility(Visibility);
	}
}

void UENTHUDManager::BindConstraintDelegates(UENTPropulsionConstraint* Constraint)
{
	if (!Constraint)
	{
		return;
	}

	Constraint->OnPropulsionStateChanged.AddDynamic(GameplayHUD, &UENTGameplayHUD::SetPropulsionActive);
}

void UENTHUDManager::OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams)
{
	if (!ActorsInitializedParams.World)
	{
		return;
	}

	if (AllHUDWidgets.Num() == 0)
	{
		CreateHUDWidgetsDelegate = ActorsInitializedParams.World->OnWorldBeginPlay.AddUObject(this, &UENTHUDManager::CreateHUDWidgets);
	}

	const UENTUIConfig* UIConfig = GetDefault<UENTUIConfig>();
	if (!UIConfig || !ActorsInitializedParams.World->GetAuthGameMode())
	{
		return;
	}

	if (!UIConfig->AllowedHUDGameModes.Contains(ActorsInitializedParams.World->GetAuthGameMode()->GetClass()))
	{
		return;
	}

	DisplayHUD();
}
