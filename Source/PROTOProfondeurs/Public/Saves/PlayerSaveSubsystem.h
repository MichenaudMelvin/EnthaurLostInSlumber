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
	virtual UDefaultSave* CreateSave(const int SaveIndex) override;

	virtual UDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);

	FDelegateHandle WorldInitDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UPlayerSave> PlayerSave;

public:
	TObjectPtr<UPlayerSave> GetPlayerSave() const {return PlayerSave;}

	UFUNCTION(BlueprintCallable, Category = "Save")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ContinueGame();
};
