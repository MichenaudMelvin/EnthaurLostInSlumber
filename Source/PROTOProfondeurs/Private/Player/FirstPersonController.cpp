// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FirstPersonController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Kevin/UI/InGameUI.h"
#include "Player/FirstPersonCharacter.h"
#include "Player/FirstPersonSpectator.h"

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

	Message += "\nbInputInteract: ";
	Message += bInputInteract ? "true" : "false";

	Message += "\bInputTakeAmber";
	Message += bInputTakeAmber ? "true" : "false";

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
	InteractAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputInteract, const FInputActionValue&);
	TakeAmberAction.FunctionName = GET_FUNCTION_NAME_CHECKED_OneParam(AFirstPersonController, OnInputTakeAmber, const FInputActionValue&);

	MoveAction.BindAction(EnhancedInputComponent, this);
	LookAction.BindAction(EnhancedInputComponent, this);
	SprintAction.BindAction(EnhancedInputComponent, this);
	CrouchAction.BindAction(EnhancedInputComponent, this);
	JumpAction.BindAction(EnhancedInputComponent, this);
	InteractAction.BindAction(EnhancedInputComponent, this);
	TakeAmberAction.BindAction(EnhancedInputComponent, this);
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

void AFirstPersonController::OnInputInteract(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputInteract = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputTakeAmber(const FInputActionValue& InputActionValue)
{
	PlayerInputs.bInputTakeAmber = InputActionValue.Get<bool>();
}

#if WITH_EDITOR
void AFirstPersonController::DisplayInputs(bool bDisplay)
{
	bDebugInputs = bDisplay;
}
#endif

#pragma endregion
