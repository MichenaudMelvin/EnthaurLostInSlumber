// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Player/ENTControllerMappingContext.h"
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
class PRFUI_API APRFUIController : public APlayerController, public IENTControllerMappingContext
{
	GENERATED_BODY()

public:
	APRFUIController();

	virtual TObjectPtr<UInputMappingContext> GetDefaultMappingContext() const override {return UIMappingContext;}

	virtual TObjectPtr<UInputMappingContext> GetUIMappingContext() const override {return UIMappingContext;}

	virtual TObjectPtr<UInputMappingContext> GetAnyKeyMappingContext() const override {return AnyKeyMappingContext;}

	virtual void SetupInputComponent() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	TObjectPtr<UInputMappingContext> AnyKeyMappingContext;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> UIMappingContext;

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

	UFUNCTION()
	void OnInputBack();
};
