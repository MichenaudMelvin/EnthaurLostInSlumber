// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/ENTLevelEntering.h"
#include "Animation/WidgetAnimation.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystems/ENTLevelNameSubsystem.h"

void UENTLevelEntering::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishedEvent;

	UpdateEvent.BindDynamic(this, &UENTLevelEntering::UnderlinesUpdate);

	UnderlinesTimeline.AddInterpFloat(UnderlinesCurve, UpdateEvent);
	UnderlinesTimeline.SetPlayRate(1 / DisplayLevelName->GetEndTime());

	if (LeftUnderline && LeftUnderline->Slot)
	{
		LeftUnderlineSlot = Cast<UCanvasPanelSlot>(LeftUnderline->Slot);
	}

	if (RightUnderline && RightUnderline->Slot)
	{
		RightUnderlineSlot = Cast<UCanvasPanelSlot>(RightUnderline->Slot);
	}
}

#if WITH_EDITOR

void UENTLevelEntering::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (!IsDesignTime() || !AreaContainer || !RegionContainer)
	{
		return;
	}

	if (LeftUnderline && LeftUnderline->Slot)
	{
		LeftUnderlineSlot = Cast<UCanvasPanelSlot>(LeftUnderline->Slot);
	}

	if (RightUnderline && RightUnderline->Slot)
	{
		RightUnderlineSlot = Cast<UCanvasPanelSlot>(RightUnderline->Slot);
	}

	if (!LeftUnderlineSlot || !RightUnderlineSlot)
	{
		return;
	}

	FMargin LeftMargin;
	FMargin RightMargin;
	GetStartMargins(LeftMargin, RightMargin);

	LeftMargin.Right = UnderlineEndPoint;
	RightMargin.Right = UnderlineEndPoint;

	LeftUnderlineSlot->SetOffsets(LeftMargin);
	RightUnderlineSlot->SetOffsets(RightMargin);
}

#endif

void UENTLevelEntering::NativeConstruct()
{
	Super::NativeConstruct();

	StopAllAnimations(); // this one is probably just for the editor when you switch to a level too fast
	PlayAnimation(DisplayLevelName);
}

void UENTLevelEntering::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UnderlinesTimeline.TickTimeline(InDeltaTime);
}

void UENTLevelEntering::SetZoneName(const FENTZoneName& ZoneName) const
{
	if (AreaName)
	{
		AreaName->SetText(ZoneName.AreaName);
	}

	if (RegionName)
	{
		RegionName->SetText(ZoneName.RegionName);
	}
}

void UENTLevelEntering::GetStartMargins(FMargin& LeftMargin, FMargin& RightMargin)
{
	if (!LeftUnderlineSlot || !RightUnderlineSlot || !AreaContainer || !RegionContainer)
	{
		return;
	}

	AreaContainer->ForceLayoutPrepass();
	RegionContainer->ForceLayoutPrepass();

	float AreaNameSize = AreaContainer->GetDesiredSize().X;
	float RegionNameSize = RegionContainer->GetDesiredSize().X;

	UnderlineStartPoint = (RegionNameSize * 0.5f) + RegionOffset;
	UnderlineEndPoint = (AreaNameSize * 0.5f) - UnderlineStartPoint;

	LeftMargin = LeftUnderlineSlot->GetOffsets();
	LeftMargin.Left = UnderlineStartPoint * -1.0f;

	RightMargin = RightUnderlineSlot->GetOffsets();
	RightMargin.Left = UnderlineStartPoint;
}

void UENTLevelEntering::StartUnderlinesLerp()
{
	if (!LeftUnderlineSlot || !RightUnderlineSlot)
	{
		return;
	}

	FMargin LeftMargin;
	FMargin RightMargin;
	GetStartMargins(LeftMargin, RightMargin);

	LeftUnderlineSlot->SetOffsets(LeftMargin);
	RightUnderlineSlot->SetOffsets(RightMargin);

	UnderlinesTimeline.PlayFromStart();
}

void UENTLevelEntering::UnderlinesUpdate(float Alpha)
{
	FMargin LeftMargin = LeftUnderlineSlot->GetOffsets();
	LeftMargin.Right = FMath::Lerp(0.0f, UnderlineEndPoint, Alpha);

	FMargin RightMargin = RightUnderlineSlot->GetOffsets();
	RightMargin.Right = FMath::Lerp(0.0f, UnderlineEndPoint, Alpha);

	LeftUnderlineSlot->SetOffsets(LeftMargin);
	RightUnderlineSlot->SetOffsets(RightMargin);
}
