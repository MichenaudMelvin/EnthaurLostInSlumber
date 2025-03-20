// Fill out your copyright notice in the Description page of Project Settings.



#include "TestMVVM/TestView.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "TestMVVM/TestModel.h"
#include "TestMVVM/TestViewModel.h"

void UTestView::NativeConstruct()
{
	Model = NewObject<UTestModel>();
	ViewModel = NewObject<UTestViewModel>();

	ViewModel->Initialize(Model); // Connect ViewModel to Model

	// Bind the ViewModel's Counter value to the UI element
	if (CounterText)
	{
		CounterText->SetText(FText::AsNumber(ViewModel->GetCounter()));
	}

	// Bind Button Click
	if (IncreaseButton)
	{
		IncreaseButton->OnClicked.AddDynamic(this, &UTestView::OnIncreaseKeyClicked);
	}

	// Listen for ViewModel's property changes (via delegates or polling)
	//ViewModel->OnModelCounterChanged.AddDynamic(this, &UTestView::UpdateCounterText);
}

void UTestView::OnIncreaseKeyClicked()
{
	if (Model)
	{
		Model->IncreaseCounter(1);
	}
}
