// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/PRFUIController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PRFUIManager.h"

void FActionUI::BindActionUI(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object)
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

APRFUIController::APRFUIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APRFUIController::BeginPlay()
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
}

void APRFUIController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if(EnhancedInputComponent == nullptr)
	{
		return;
	}

	ResumeAction.FunctionNameUI = GET_FUNCTION_NAME_CHECKED(APRFUIController, OnInputResume);
	AnyAction.FunctionNameUI = GET_FUNCTION_NAME_CHECKED(APRFUIController, OnInputAny);
	
	ResumeAction.BindActionUI(EnhancedInputComponent, this);
	AnyAction.BindActionUI(EnhancedInputComponent, this);
}

void APRFUIController::OnInputResume()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	UIManager->CloseAllMenus();
}

void APRFUIController::OnInputAny()
{
	UPRFUIManager* UIManager = GetGameInstance()->GetSubsystem<UPRFUIManager>();
	if (!IsValid(UIManager))
	{
		return;
	}

	if (UIManager->GetMenuState() != EPRFUIState::AnyMenu)
	{
		return;
	}

	UIManager->OpenMenu(UIManager->GetMainMenu(), false);
	UIManager->SetMenuState(EPRFUIState::MainMenu);
}

// Called every frame
void APRFUIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

