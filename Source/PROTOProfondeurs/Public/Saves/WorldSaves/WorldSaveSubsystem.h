// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Saves/SaveSubsystem.h"
#include "WorldSaveSubsystem.generated.h"

class UWorldSave;

UCLASS()
class PROTOPROFONDEURS_API UWorldSaveSubsystem : public USaveSubsystem
{
	GENERATED_BODY()

public:
	UWorldSaveSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	virtual UDefaultSave* CreateSave(const int SaveIndex) override;

	virtual UDefaultSave* SaveToSlot(const int SaveIndex) override;

	virtual UDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true) override;

	UFUNCTION(BlueprintCallable, Category = "World")
	bool DeleteWorldSaveByName(const FString& WorldName, const int SaveIndex);

	UFUNCTION(BlueprintCallable, Category = "World")
	void DeleteAllWorldSaves(const int SaveIndex);

protected:
	FDelegateHandle WorldInitDelegateHandle;

	FDelegateHandle WorldBeginPlayDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "World")
	TObjectPtr<UWorldSave> CurrentWorldSave;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);

	void OnNewWorldBeginPlay();

public:
	TObjectPtr<UWorldSave> GetCurrentWorldSave() const {return CurrentWorldSave;}
};
