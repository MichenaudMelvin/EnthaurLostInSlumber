// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "FirstPersonController.generated.h"

class AFirstPersonSpectator;
class AFirstPersonCharacter;
class UInGameUI;
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

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	FVector2D InputMove = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	FVector2D InputLook = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputSprint = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputCrouch = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputJump = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputInteractPressed = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputInteractTrigger = false;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	bool bInputPauseGame = false;

#if !UE_BUILD_SHIPPING
	void DisplayInputsOnScreen(float DisplayTime = 0.0f, const FColor& DebugColor = FColor::Cyan) const;
#endif
};

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonController : public APlayerController
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> InGameWidgetClass;

	UPROPERTY()
	TObjectPtr<UInGameUI> CurrentInGameUI;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> DeathWidgetClass;

	UPROPERTY()
	TObjectPtr<class UDeathMenuUI> CurrentDeathUI;

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

public:
	UInGameUI* GetCurrentInGameUI() { return CurrentInGameUI; }

#pragma region Inputs

protected:
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction SprintAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction CrouchAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction InteractPressedAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction InteractTriggerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	FAction PauseGameAction;

	UPROPERTY(BlueprintReadOnly, Category = "Inputs")
	FPlayerInputs PlayerInputs;

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
	void OnInputPauseGame(const FInputActionValue& InputActionValue);

public:
	const FPlayerInputs& GetPlayerInputs() const {return PlayerInputs;}

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

#pragma region UI Menus

#pragma endregion

#pragma region Respawn

	UFUNCTION(BlueprintCallable)
	void KillPlayer();

	UFUNCTION(BlueprintCallable)
	void RespawnPlayer(FVector RespawnPosition);

#pragma endregion 
};
