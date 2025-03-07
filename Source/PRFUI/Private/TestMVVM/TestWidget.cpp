// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMVVM/TestWidget.h"
#include "TestMVVM/TestViewModel.h"
#include "Components/TextBlock.h"

void UTestWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Create and assign ViewModel
	ViewModel = NewObject<UTestViewModel>();
	ViewModel->OnCounterUpdated.AddDynamic(this, &UTestWidget::UpdateCounterText);

	UpdateCounterText();
}

void UTestWidget::UpdateCounterText()
{
	if (CounterText && ViewModel)
	{
		CounterText->SetText(ViewModel->GetCounterText());
	}
}