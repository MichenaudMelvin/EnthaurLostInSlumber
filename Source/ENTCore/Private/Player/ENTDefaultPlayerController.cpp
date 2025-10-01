// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/ENTDefaultPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Player/ENTDefaultCharacter.h"

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

void AENTDefaultPlayerController::BeginPlay()
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

	// TODO check this
	// UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	// if (!IsValid(UIManager))
	// {
	// 	return;
	// }
	//
	// UIManager->SetMenuState(EPRFUIState::Gameplay);
}

void AENTDefaultPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (PlayerInputs.bInputInteractPressed)
	{
		PressedDuration += DeltaSeconds;
		if (PressedDuration >= MaxPressedDuration)
		{
			PressedDuration = 0.0f;
			PlayerInputs.bInputInteractPressed = false;
		}
	}

#if WITH_EDITORONLY_DATA
	if (bDebugInputs)
	{
		PlayerInputs.DisplayInputsOnScreen();
	}
#endif
}

#pragma region Inputs

void AENTDefaultPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if(EnhancedInputComponent == nullptr)
	{
		return;
	}

	MoveAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputMove, const FInputActionValue&);
	LookAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputLook, const FInputActionValue&);
	SprintAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputSprint, const FInputActionValue&);
	CrouchAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputCrouch, const FInputActionValue&);
	JumpAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputJump, const FInputActionValue&);
	InteractPressedAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputInteractPressed, const FInputActionValue&);
	InteractTriggerAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AENTDefaultPlayerController, OnInputInteractTrigger, const FInputActionValue&);
	PauseGameAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AENTDefaultPlayerController, OnInputPauseGame);

	NavigateAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AENTDefaultPlayerController, OnInputNavigate);
	SelectAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AENTDefaultPlayerController, OnInputSelect);
	BackAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AENTDefaultPlayerController, OnInputBack);
	ResumeAction.FunctionName = GET_FUNCTION_NAME_CHECKED(AENTDefaultPlayerController, OnInputResume);

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

void AENTDefaultPlayerController::OnInputMove(const FInputActionValue& InputActionValue)
{
	PlayerInputs.InputMove = InputActionValue.Get<FVector2D>();
}

void AENTDefaultPlayerController::OnInputLook(const FInputActionValue& InputActionValue)
{
	PlayerInputs.InputLook = InputActionValue.Get<FVector2D>();
}

void AENTDefaultPlayerController::OnInputSprint(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputSprint = InputActionValue.Get<bool>();
}

void AENTDefaultPlayerController::OnInputCrouch(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputCrouch = InputActionValue.Get<bool>();
}

void AENTDefaultPlayerController::OnInputJump(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputJump = InputActionValue.Get<bool>();
}

void AENTDefaultPlayerController::OnInputInteractPressed(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputInteractPressed = InputActionValue.Get<bool>();
}

void AENTDefaultPlayerController::OnInputInteractTrigger(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputInteractTrigger = InputActionValue.Get<bool>();
}

void AENTDefaultPlayerController::OnInputPauseGame(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputPauseGame = InputActionValue.Get<bool>();
	OnPauseGame.Broadcast();
}

void AENTDefaultPlayerController::OnInputNavigate(const FInputActionValue& InputActionValue)
{
	OnNavigate.Broadcast(InputActionValue.Get<FVector2D>());
}

void AENTDefaultPlayerController::OnInputSelect()
{
	OnSelect.Broadcast();
}

void AENTDefaultPlayerController::OnInputBack()
{
	OnBack.Broadcast();
}

void AENTDefaultPlayerController::OnInputResume()
{
	OnResume.Broadcast();
}

#if WITH_EDITOR
void AENTDefaultPlayerController::DisplayInputs(bool bDisplay)
{
	bDebugInputs = bDisplay;
}
#endif

#pragma endregion
