// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/ENTSaveSubsystem.h"
#include "ENTWorldSaveSubsystem.generated.h"

class UENTWorldSave;

UCLASS()
class ENTSAVES_API UENTWorldSaveSubsystem : public UENTSaveSubsystem
{
	GENERATED_BODY()

public:
	UENTWorldSaveSubsystem();

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	virtual UENTDefaultSave* CreateSave(const int SaveIndex) override;

	virtual UENTDefaultSave* SaveToSlot(const int SaveIndex) override;

	virtual UENTDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true) override;

	UFUNCTION(BlueprintCallable, Category = "World")
	bool DeleteWorldSaveByName(const FString& WorldName, const int SaveIndex);

	UFUNCTION(BlueprintCallable, Category = "World")
	void DeleteAllWorldSaves(const int SaveIndex);

protected:
	FDelegateHandle WorldInitDelegateHandle;

	FDelegateHandle WorldBeginPlayDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "World")
	TObjectPtr<UENTWorldSave> CurrentWorldSave;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);

	void OnNewWorldBeginPlay();

public:
	TObjectPtr<UENTWorldSave> GetCurrentWorldSave() const {return CurrentWorldSave;}
};
