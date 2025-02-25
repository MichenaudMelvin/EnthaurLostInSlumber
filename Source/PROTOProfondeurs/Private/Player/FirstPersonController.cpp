// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FirstPersonController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

void FAction::BindAction(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object)
{
	if (Action == nullptr)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Action is nullptr for the target function: %s"), *FunctionName.ToString());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	if (Object == nullptr)
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

	GEngine->AddOnScreenDebugMessage(-1, DisplayTime, DebugColor, FString::Printf(TEXT("%s"), *Message));
}
#endif

void AFirstPersonController::BeginPlay()
{
	Super::BeginPlay();

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (LocalPlayer == nullptr)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if(Subsystem == nullptr)
	{
		return;
	}

	if (DefaultMappingContext == nullptr)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("Missing DefaultMappingContext in %s"), *GetClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
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

	MoveAction.BindAction(EnhancedInputComponent, this);
	LookAction.BindAction(EnhancedInputComponent, this);
	SprintAction.BindAction(EnhancedInputComponent, this);
	CrouchAction.BindAction(EnhancedInputComponent, this);
	JumpAction.BindAction(EnhancedInputComponent, this);
	InteractAction.BindAction(EnhancedInputComponent, this);
}

void AFirstPersonController::OnInputMove(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Axis2D)
	{
		const FString Message = FString::Printf(TEXT("Move action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.InputMove = InputActionValue.Get<FVector2D>();
}

void AFirstPersonController::OnInputLook(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Axis2D)
	{
		const FString Message = FString::Printf(TEXT("Look action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.InputLook = InputActionValue.Get<FVector2D>();
}

void AFirstPersonController::OnInputSprint(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Boolean)
	{
		const FString Message = FString::Printf(TEXT("Sprint action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.bInputSprint = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputCrouch(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Boolean)
	{
		const FString Message = FString::Printf(TEXT("Crouch action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.bInputCrouch = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputJump(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Boolean)
	{
		const FString Message = FString::Printf(TEXT("Jump action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.bInputJump = InputActionValue.Get<bool>();
}

void AFirstPersonController::OnInputInteract(const FInputActionValue& InputActionValue)
{
#if WITH_EDITOR
	if (InputActionValue.GetValueType() != EInputActionValueType::Boolean)
	{
		const FString Message = FString::Printf(TEXT("Interact action has wrong Value type"));

		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
		return;
	}
#endif

	PlayerInputs.bInputInteract = InputActionValue.Get<bool>();
}

#if WITH_EDITOR
void AFirstPersonController::DisplayInputs(bool bDisplay)
{
	bDebugInputs = bDisplay;
}
#endif

#pragma endregion
