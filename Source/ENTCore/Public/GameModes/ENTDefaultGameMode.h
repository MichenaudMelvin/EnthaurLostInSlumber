// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ENTDefaultGameMode.generated.h"

class AENTDefaultCharacter;

UCLASS()
class ENTCORE_API AENTDefaultGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Exec, Category = "WorldSave")
	void SaveWorld() const;

	UFUNCTION(BlueprintCallable, Exec, Category = "WorldSave")
	void LoadWorld() const;
};
