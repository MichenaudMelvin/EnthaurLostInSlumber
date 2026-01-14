// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/ENTSaveSubsystem.h"
#include "ENTWorldSaveSubsystem.generated.h"

class UENTWorldSave;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFinishLoading, UENTWorldSave*, WorldSave);

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

	UPROPERTY(BlueprintAssignable, Category = "World")
	FOnFinishLoading OnFinishLoading;

protected:
	FDelegateHandle WorldInitDelegateHandle;

	FDelegateHandle WorldBeginPlayDelegateHandle;

	FDelegateHandle WorldBeginTearDownDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "World")
	TObjectPtr<UENTWorldSave> CurrentWorldSave;

	int32 LoadedLevelIndex = 0;

	bool bLoadedPlayer = false;

	bool bLoadSingleLevel = false;

	bool bIsLoading = false;

	bool bIsLoaded = false;

	bool bCannotLoadWorld = false;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);

	void UnloadSublevels();

	UFUNCTION()
	void LoadSublevels();

	UFUNCTION()
	void FinishLoading();

	void OnNewWorldBeginPlay();

	void OnWorldBeginTearDown(UWorld* World);

public:
	TObjectPtr<UENTWorldSave> GetCurrentWorldSave() const {return CurrentWorldSave;}

	bool IsLoading() const {return bIsLoading;}

	bool IsLoaded() const {return bIsLoaded;}
};
