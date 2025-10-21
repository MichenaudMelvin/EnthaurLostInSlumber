// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ENTPawnAIInterface.generated.h"

class UBehaviorTree;
struct FENTAIData;

// This class does not need to be modified.
UINTERFACE()
class ENTARTIFICIALINTELLIGENCE_API UENTPawnAIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ENTARTIFICIALINTELLIGENCE_API IENTPawnAIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "AI|Behavior")
	bool DoesAutoStartBehaviorTree() const;

	/**
	 * @brief Return the overridenBehaviorTree of the pawn, if nullptr use the behaviorTree of the controller
	 * @return 
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AI|Behavior")
	UBehaviorTree* GetOverridenBehaviorTree() const;

	/**
	 * @brief When the function RunBehaviorTree is called in the controller
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "AI|Behavior")
	void OnBehaviorTreeStarted();

	virtual bool HasReceivedLoadingRequest() const = 0;

	virtual const FENTAIData& GetLoadingData() const = 0; 
};
