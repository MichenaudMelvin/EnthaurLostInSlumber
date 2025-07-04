// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menus/Elements/PRFCustomButton.h"
#include "DeathMenuUI.generated.h"

UCLASS()
class PROTOPROFONDEURS_API UDeathMenuUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintCallable, Category = "Player")
	void RespawnPlayer();
};
