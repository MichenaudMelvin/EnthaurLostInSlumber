// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMVVM/TestModel.h"

UTestModel::UTestModel()
{
	Counter = 0;
}

void UTestModel::IncreaseCounter(int32 Amount)
{
	Counter += Amount;
	OnCounterChanged.Broadcast(Counter);
}

int32 UTestModel::GetCounter() const
{
	return Counter;
}
