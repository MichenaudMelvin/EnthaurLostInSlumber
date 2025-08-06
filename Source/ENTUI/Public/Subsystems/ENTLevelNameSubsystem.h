// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Config/ENTUIConfig.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ENTLevelNameSubsystem.generated.h"

UCLASS()
class ENTUI_API UENTLevelNameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

	void SetupWorldName(const FActorsInitializedParams& ActorsInitializedParams);

	FDelegateHandle WorldInitDelegateHandle;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "LevelName")
	FENTZoneName CurrentZoneName;

public:
	FENTZoneName GetZoneName() const { return CurrentZoneName; }
};
