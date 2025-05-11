// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveSubsystem.h"
#include "PlayerSaveSubsystem.generated.h"

class UPlayerSave;

UCLASS()
class PROTOPROFONDEURS_API UPlayerSaveSubsystem : public USaveSubsystem
{
	GENERATED_BODY()

public:
	UPlayerSaveSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UPlayerSave> PlayerSave;

public:
	TObjectPtr<UPlayerSave> GetPlayerSave() const {return PlayerSave;}
};
