// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "TestModel.h"
#include "UObject/Object.h"
#include "TestViewModel.generated.h"

/**
 * 
 */
UCLASS()
class PRFUI_API UTestViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	void Initialize(UTestModel* InModel);
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintPure)
	int32 GetCounter() const;

protected:
	UPROPERTY(BlueprintReadOnly, FieldNotify, Getter)
	int32 Counter = 1;

private:
	UPROPERTY()
	TObjectPtr<UTestModel> Model;

	UFUNCTION()
	void OnModelCounterChanged(const int32 InValue);
};
