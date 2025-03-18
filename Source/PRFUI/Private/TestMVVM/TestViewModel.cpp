// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMVVM/TestViewModel.h"

void UTestViewModel::Initialize(UTestModel* InModel)
{
	if (!IsValid(InModel))
	{
		return;
	}

	Model = InModel;
	Counter = Model->GetCounter();

	Model->OnCounterChanged.AddDynamic(this, &UTestViewModel::OnModelCounterChanged);
}

void UTestViewModel::BeginDestroy()
{
	if (Model)
	{
		Model->OnCounterChanged.RemoveDynamic(this, &UTestViewModel::OnModelCounterChanged);
	}

	Super::BeginDestroy();
}

int32 UTestViewModel::GetCounter() const
{
	return Counter;
}

void UTestViewModel::OnModelCounterChanged(const int32 InValue)
{
	Counter = InValue;
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Counter);
}
