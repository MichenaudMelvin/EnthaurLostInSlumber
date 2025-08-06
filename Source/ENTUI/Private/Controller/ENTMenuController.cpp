// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ENTMenuController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Subsystems/ENTMenuManager.h"

void FENTActionUI::BindActionUI(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object)
{
	if (!ActionUI)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Action is nullptr for the target function: %s"), *FunctionNameUI.ToString());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	if (!Object)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Object is nullptr for the target function: %s"), *FunctionNameUI.ToString());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	for (const ETriggerEvent& Event : TriggerEventsUI)
	{
		EnhancedInputComponent->BindAction(ActionUI, Event, Object, FunctionNameUI);
	}
}

AENTMenuController::AENTMenuController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AENTMenuController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(!Subsystem)
	{
		return;
	}

	if (!AnyKeyMappingContext)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Missing AnyKeyMappingContext in %s"), *GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	Subsystem->AddMappingContext(AnyKeyMappingContext, 0);
}

void AENTMenuController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!EnhancedInputComponent)
	{
		return;
	}

	ResumeAction.FunctionNameUI = GET_FUNCTION_NAME_CHECKED(AENTMenuController, OnInputResume);
	AnyAction.FunctionNameUI = GET_FUNCTION_NAME_CHECKED(AENTMenuController, OnInputAny);
	BackAction.FunctionNameUI = GET_FUNCTION_NAME_CHECKED(AENTMenuController, OnInputBack);

	ResumeAction.BindActionUI(EnhancedInputComponent, this);
	AnyAction.BindActionUI(EnhancedInputComponent, this);
	BackAction.BindActionUI(EnhancedInputComponent, this);
}

void AENTMenuController::OnInputResume()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	if (UIManager->GetMenuState() == EENTMenuState::MainMenu)
	{
		UIManager->CloseCurrentMenu();
		return;
	}

	UIManager->CloseAllMenus(EENTMenuState::Gameplay);
}

void AENTMenuController::OnInputAny()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	if (UIManager->GetMenuState() != EENTMenuState::AnyMenu)
	{
		return;
	}

	UIManager->OpenMenu(UIManager->GetMainMenu(), false);
}

void AENTMenuController::OnInputBack()
{
	UENTMenuManager* UIManager = GetGameInstance()->GetSubsystem<UENTMenuManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseCurrentMenu();
}
