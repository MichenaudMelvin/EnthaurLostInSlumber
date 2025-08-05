// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ENTDeathTransition.generated.h"

UCLASS()
class PRFUI_API UENTDeathTransition : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RespawnPlayer();
};
