// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ENTSaveSubsystem.h"
#include "ENTPlayerSaveSubsystem.generated.h"

class UENTPlayerSave;

UCLASS()
class ENTSAVES_API UENTPlayerSaveSubsystem : public UENTSaveSubsystem
{
	GENERATED_BODY()

public:
	UENTPlayerSaveSubsystem();

protected:
	virtual UENTDefaultSave* CreateSave(const int SaveIndex) override;

	virtual UENTDefaultSave* LoadSave(const int SaveIndex, const bool bCreateNewSaveIfDoesntExist = true) override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void OnNewWorldStarted(const FActorsInitializedParams& ActorsInitializedParams);

	FDelegateHandle WorldInitDelegateHandle;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	TObjectPtr<UENTPlayerSave> PlayerSave;

public:
	TObjectPtr<UENTPlayerSave> GetPlayerSave() const {return PlayerSave;}

	UFUNCTION(BlueprintCallable, Category = "Save")
	void StartNewGame();

	UFUNCTION(BlueprintCallable, Category = "Save")
	void ContinueGame();
};
