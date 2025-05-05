// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FirstPersonController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UIManagerSettings.h"
#include "Blueprint/UserWidget.h"
#include "UI/DeathMenuUI.h"
#include "UI/InGameUI.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonSpectator.h"
#include "PRFUI/Public/PRFUIManager.h"

FAction::FAction()
{
	TriggerEvents.Add(ETriggerEvent::Triggered);
	TriggerEvents.Add(ETriggerEvent::Started);
	TriggerEvents.Add(ETriggerEvent::Completed);
}

void FAction::BindAction(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object)
{
	if (!Action)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Action is nullptr for the target function: %s"), *FunctionName.ToString());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	if (!Object)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Object is nullptr for the target function: %s"), *FunctionName.ToString());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	for (const ETriggerEvent& Event : TriggerEvents)
	{
		EnhancedInputComponent->BindAction(Action, Event, Object, FunctionName);
	}
}

#if !UE_BUILD_SHIPPING
void FPlayerInputs::DisplayInputsOnScreen(float DisplayTime, const FColor& DebugColor) const
{
	FString Message;
	Message += "InputMove: " + InputMove.ToString();
	Message += "\nInputLook: " + InputLook.ToString();

	Message += "\nbInputSprint: ";
	Message += bInputSprint ? "true" : "false";

	Message += "\nbInputCrouch: ";
	Message += bInputCrouch ? "true" : "false";

	Message += "\nbInputJump: ";
	Message += bInputJump ? "true" : "false";

	Message += "\nbInputInteractPressed: ";
	Message += bInputInteractPressed ? "true" : "false";

	Message += "\nbInputInteractTrigger: ";
	Message += bInputInteractTrigger ? "true" : "false";

	GEngine->AddOnScreenDebugMessage(-1, DisplayTime, DebugColor, FString::Printf(TEXT("%s"), *Message));
}
#endif

void AFirstPersonController::BeginPlay()
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

	if (!DefaultMappingContext)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Missing DefaultMappingContext in %s"), *GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
	OwnCharacter = Cast<AFirstPersonCharacter>(GetPawn());

	if (!SpectatorClass)
	{
		SpectatorClass = AFirstPersonSpectator::StaticClass();
	}

	CurrentInGameUI = CreateWidget<UInGameUI>(this, InGameWidgetClass);
	if (CurrentInGameUI)
	{
		CurrentInGameUI->AddToViewport();
	}
}

void AFirstPersonController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if WITH_EDITORONLY_DATA
	if (bDebugInputs)
	{
		PlayerInputs.DisplayInputsOnScreen();
	}
#endif
}

// #if !UE_BUILD_SHIPPING
void AFirstPersonController::PossessSpectator()
{
	Spectator = GetWorld()->SpawnActor<AFirstPersonSpectator>(SpectatorClass, GetPawn()->GetActorLocation(), GetPawn()->GetActorRotation());
	UnPossess();
	Possess(Spectator);
}

void AFirstPersonController::UnPossessSpectator(bool bTeleport)
{
	if (!Spectator)
	{
		return;
	}

	if (bTeleport)
	{
		OwnCharacter->SetActorLocation(Spectator->GetActorLocation());
	}

	UnPossess();
	Possess(OwnCharacter);

	Spectator->Destroy();
}
// #endif

#pragma region Inputs

void AFirstPersonController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if(EnhancedInputComponent == nullptr)
	{
		return;
	}

	MoveAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputMove, const FInputActionValue&);
	LookAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputLook, const FInputActionValue&);
	SprintAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputSprint, const FInputActionValue&);
	CrouchAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputCrouch, const FInputActionValue&);
	JumpAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputJump, const FInputActionValue&);
	InteractPressedAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputInteractPressed, const FInputActionValue&);
	InteractTriggerAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputInteractTrigger, const FInputActionValue&);
	PauseGameAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AFirstPersonController, OnInputPauseGame);

	NavigateAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AFirstPersonController, OnInputNavigate);
	SelectAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AFirstPersonController, OnInputSelect);
	BackAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AFirstPersonController, OnInputBack);
	ResumeAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AFirstPersonController, OnInputResume);

	MoveAction.BindAction(EnhancedInputComponent, this);
	LookAction.BindAction(EnhancedInputComponent, this);
	SprintAction.BindAction(EnhancedInputComponent, this);
	CrouchAction.BindAction(EnhancedInputComponent, this);
	JumpAction.BindAction(EnhancedInputComponent, this);
	InteractPressedAction.BindAction(EnhancedInputComponent, this);
	InteractTriggerAction.BindAction(EnhancedInputComponent, this);
	PauseGameAction.BindAction(EnhancedInputComponent, this);
	
	NavigateAction.BindAction(EnhancedInputComponent, this);
	SelectAction.BindAction(EnhancedInputComponent, this);
	BackAction.BindAction(EnhancedInputComponent, this);
	ResumeAction.BindAction(EnhancedInputComponent, this);
}

void AFirstPersonController::OnInputMove(const FInputActionValue& InputActionValue)
{
	PlayerInputs.InputMove = InputActionValue.Get<FVector2D>();
}

void AFirstPersonController::OnInputLook(const FInputActionValue& InputActionValue)
{
	PlayerInputs.InputLook = InputActionValue.Get<FVector2D>();
}

void AFirstPersonController::OnInputSprint(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputSprint = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputCrouch(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputCrouch = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputJump(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputJump = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputInteractPressed(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputInteractPressed = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputInteractTrigger(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputInteractTrigger = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputPauseGame()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->SetMenuState(EPRFUIState::PauseMenu);
	UIManager->OpenMenu(UIManager->GetPauseMenu(), false);
}

void AFirstPersonController::OnInputNavigate(const FInputActionValue& InputActionValue)
{
	
}

void AFirstPersonController::OnInputSelect()
{
	
}

void AFirstPersonController::OnInputBack()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseCurrentMenu();
	
}

void AFirstPersonController::OnInputResume()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	switch (UIManager->GetMenuState())
	{
		case EPRFUIState::PauseMenu:
			UIManager->SetMenuState(EPRFUIState::Gameplay);
        	UIManager->CloseAllMenus();
			break;

		case EPRFUIState::MainMenu:
			UIManager->CloseCurrentMenu();
			break;

		default:
			break;
	}
}

#if WITH_EDITOR
void AFirstPersonController::DisplayInputs(bool bDisplay)
{
	bDebugInputs = bDisplay;
}
#endif

#pragma endregion

void AFirstPersonController::KillPlayer()
{
	SetPause(true);
	CurrentDeathUI = CreateWidget<UDeathMenuUI>(this, DeathWidgetClass);
	if (CurrentDeathUI)
	{
		CurrentDeathUI->AddToViewport();
	}
}

void AFirstPersonController::RespawnPlayer(FVector RespawnPosition)
{
}
