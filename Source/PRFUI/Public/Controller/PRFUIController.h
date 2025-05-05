// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/Controller.h"
#include "PRFUIController.generated.h"

USTRUCT(Blueprintable)
struct FActionUI
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TObjectPtr<UInputAction> ActionUI;

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TArray<ETriggerEvent> TriggerEventsUI;

	/**
	 * @brief Function needs to be a UFunction
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	FName FunctionNameUI = NAME_None;

	void BindActionUI(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object);
};

UCLASS()
class PRFUI_API APRFUIController : public APlayerController
{
	GENERATED_BODY()

public:
	APRFUIController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FActionUI ResumeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FActionUI AnyAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FActionUI BackAction;

	UFUNCTION()
	void OnInputResume();

	UFUNCTION()
	void OnInputAny();

public:
	virtual void Tick(float DeltaTime) override;
};
