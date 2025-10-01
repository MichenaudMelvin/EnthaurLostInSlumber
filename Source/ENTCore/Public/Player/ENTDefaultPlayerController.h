// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "ENTControllerMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "ENTDefaultPlayerController.generated.h"

class AFirstPersonSpectator;
class AENTDefaultCharacter;
struct FInputActionValue;
class UInputAction;
enum class ETriggerEvent : uint8;
class UInputMappingContext;

USTRUCT(Blueprintable)
struct FAction
{
	GENERATED_BODY()

	FAction();

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TArray<ETriggerEvent> TriggerEvents;

	/**
	 * @brief Function needs to be a UFunction
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	FName FunctionName = NAME_None;

	void BindAction(UEnhancedInputComponent* EnhancedInputComponent, UObject* Object);
};

USTRUCT(Blueprintable)
struct FPlayerInputs
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	FVector2D InputMove = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	FVector2D InputLook = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputSprint = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputCrouch = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputJump = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputInteractPressed = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputInteractTrigger = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs Default")
	bool bInputPauseGame = false;

#if !UE_BUILD_SHIPPING
	void DisplayInputsOnScreen(float DisplayTime = 0.0f, const FColor& DebugColor = FColor::Cyan) const;
#endif
};

# pragma region UI Related Deletages

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPauseGame);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNavigate, FVector2D, Axis);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSelect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBack);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnResume);

#pragma endregion

UCLASS()
class ENTCORE_API AENTDefaultPlayerController : public APlayerController, public IENTControllerMappingContext
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Inputs

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	FPlayerInputs PlayerInputs;

#pragma region IMC_Default

public:
	TObjectPtr<UInputMappingContext> GetDefaultMappingContext() { return DefaultMappingContext; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction InteractPressedAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction InteractTriggerAction;

	float PressedDuration = 0.0f;

	/**
	 * @brief Due to the UE 5.6 migration, might be a temporary solution
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	float MaxPressedDuration = (1.0f/60.0f) * 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs Default")
	FAction PauseGameAction;

	UFUNCTION()
	void OnInputMove(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputLook(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputSprint(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputCrouch(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputJump(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputInteractPressed(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputInteractTrigger(const FInputActionValue& InputActionValue);

#pragma endregion

#pragma region IMC_UI

public:
	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnPauseGame OnPauseGame;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnNavigate OnNavigate;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnSelect OnSelect;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnBack OnBack;

	UPROPERTY(BlueprintAssignable, Category = "UI|Delegates")
	FOnResume OnResume;

	virtual TObjectPtr<UInputMappingContext> GetDefaultMappingContext() const override {return DefaultMappingContext;}

	virtual TObjectPtr<UInputMappingContext> GetUIMappingContext() const override {return UIMappingContext;}

	virtual TObjectPtr<UInputMappingContext> GetAnyKeyMappingContext() const override {return AnyKeyMappingContext;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	TObjectPtr<UInputMappingContext> UIMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	TObjectPtr<UInputMappingContext> AnyKeyMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	FAction NavigateAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	FAction SelectAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	FAction BackAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs UI")
	FAction ResumeAction;

	UFUNCTION()
	void OnInputPauseGame(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputNavigate(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnInputSelect();

	UFUNCTION()
	void OnInputBack();

	UFUNCTION()
	void OnInputResume();

#pragma endregion

public:
	const FPlayerInputs& GetPlayerInputs() const {return PlayerInputs;}

	void ClearPlayerInputs() { PlayerInputs = FPlayerInputs(); }

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inputs|Debug")
	bool bDebugInputs = false;
#endif

#if WITH_EDITOR
	UFUNCTION(Exec)
	void DisplayInputs(bool bDisplay);
#endif

#pragma endregion
};
