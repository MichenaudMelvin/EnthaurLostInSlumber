// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestViewModel.h"
#include "GameFramework/Actor.h"
#include "UObject/Object.h"
#include "TestInputHandler.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API ATestInputHandler : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UTestViewModel* ViewModel;

public:
	ATestInputHandler();

	void SetViewModel(UTestViewModel* InViewModel);

protected:
	virtual void BeginPlay() override;

	void OnIncreaseCounterPressed();
};