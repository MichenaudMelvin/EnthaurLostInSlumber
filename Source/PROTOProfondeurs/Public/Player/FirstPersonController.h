// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FirstPersonController.generated.h"

struct FInputActionValue;
class UInputAction;
enum class ETriggerEvent : uint8;
class UInputMappingContext;

USTRUCT(Blueprintable)
struct FAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	TArray<ETriggerEvent> TriggerEvents;

	/**
	 * @brief Function needs to be a UFunction
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Inputs")
	FName FunctionName;

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
	bool bInputInteract = false;

#if !UE_BUILD_SHIPPING
	void DisplayInputsOnScreen(float DisplayTime = 0.0f, const FColor& DebugColor = FColor::Cyan) const;
#endif
};

UCLASS()
class PROTOPROFONDEURS_API AFirstPersonController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

#pragma region Inputs
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
	FAction InteractAction;

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
	void OnInputInteract(const FInputActionValue& InputActionValue);

public:
	const FPlayerInputs& GetPlayerInputs() const {return PlayerInputs;}

#if WITH_EDITORONLY_DATA
private:
	UPROPERTY(EditDefaultsOnly, Category = "Inputs|Debug")
	bool bDebugInputs = false;

	UFUNCTION(Exec)
	void DisplayInputs(bool bDisplay);
#endif

#pragma endregion
};
