// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Controller/PRFControllerMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "FirstPersonController.generated.h"

class AFirstPersonSpectator;
class AFirstPersonCharacter;
class UInGameUI;
class UDeathMenuUI;
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

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonController : public APlayerController, public IPRFControllerMappingContext
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

// #if !UE_BUILD_SHIPPING
	UPROPERTY()
	TObjectPtr<AFirstPersonCharacter> OwnCharacter;

	UPROPERTY()
	TObjectPtr<AFirstPersonSpectator> Spectator;

	UPROPERTY(EditDefaultsOnly, Category = "Spectator")
	TSubclassOf<AFirstPersonSpectator> SpectatorClass;

	UFUNCTION(Exec)
	void PossessSpectator();

	UFUNCTION(Exec)
	void UnPossessSpectator(bool bTeleport = true);
// #endif

#pragma region UI

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInGameUI> InGameWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInGameUI> CurrentInGameUI;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeathMenuUI> DeathWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UDeathMenuUI> CurrentDeathUI;

public:
	TObjectPtr<UInGameUI> GetCurrentInGameUI() {return CurrentInGameUI;}

	TObjectPtr<UDeathMenuUI> GetDeathMenuUI() {return CurrentDeathUI;}

#pragma endregion

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

	UFUNCTION()
	void OnInputPauseGame();

#pragma endregion

#pragma region IMC_UI

public:
	virtual TObjectPtr<UInputMappingContext> GetUIMappingContext() const override;
	virtual TObjectPtr<UInputMappingContext> GetAnyKeyMappingContext() const override;
	virtual TObjectPtr<UInputMappingContext> GetDefaultMappingContext() const override;

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
