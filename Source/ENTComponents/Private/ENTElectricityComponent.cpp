#include "ENTElectricityComponent.h"
#include "ENTElectricityFeedback.h"


UENTElectricityComponent::UENTElectricityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

bool UENTElectricityComponent::IsAnimRunning() const
{
	return IsValid(ElectricityFeedback);
}

void UENTElectricityComponent::BeginPlay()
{
	Super::BeginPlay();

	FOnTimelineFloat UpdateEvent;
	FOnTimelineEvent FinishedEvent;

	UpdateEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityRadiusUpdate);
	FinishedEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityRadiusFinished);

	ElectricityRadiusTimeline.AddInterpFloat(FirstElectricityRadiusCurve, UpdateEvent, "PropertyRadius", "TrackRadius");
	ElectricityRadiusTimeline.SetTimelineFinishedFunc(FinishedEvent);

	UpdateEvent.Unbind();
	FinishedEvent.Unbind();

	UpdateEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityMovementUpdate);
	FinishedEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityMovementFinished);

	ElectricityMovementTimeline.SetPlayRate(1 / ElectricityMovementDuration);
	ElectricityMovementTimeline.AddInterpFloat(ElectricityMovementCurve, UpdateEvent);
	ElectricityMovementTimeline.SetTimelineFinishedFunc(FinishedEvent);

	UpdateEvent.Unbind();
	FinishedEvent.Unbind();

	UpdateEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityOpacityUpdate);
	FinishedEvent.BindDynamic(this, &UENTElectricityComponent::ElectricityOpacityFinished);

	ElectricityOpacityTimeline.SetPlayRate(1 / ElectricityOpacityDuration);
	ElectricityOpacityTimeline.AddInterpFloat(ElectricityOpacityCurve, UpdateEvent);
	ElectricityOpacityTimeline.SetTimelineFinishedFunc(FinishedEvent);
}

void UENTElectricityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ElectricityRadiusTimeline.TickTimeline(DeltaTime);
	ElectricityMovementTimeline.TickTimeline(DeltaTime);
	ElectricityOpacityTimeline.TickTimeline(DeltaTime);
}

void UENTElectricityComponent::PlayElectricityAnimation(AActor* LinkedActor)
{
	OnElectricityAnimationStarted.Broadcast(LinkedActor);
	PlayElectricityAnimation(LinkedActor->GetTransform());
}

void UENTElectricityComponent::PlayElectricityAnimation(const FTransform& Transform)
{
	if (!ElectricityFeedbackClass)
	{
		return;
	}

	ElectricityFeedback = GetWorld()->SpawnActor<AENTElectricityFeedback>(ElectricityFeedbackClass, Transform);
	ElectricityFeedback->Init(ElectricityColor);

	if (!ElectricityFeedback)
	{
		return;
	}

	StartRadiusTarget = 0.0f;
	EndRadiusTarget = FirstRadiusTarget;
	ElectricityRadiusTimeline.SetFloatCurve(FirstElectricityRadiusCurve, "TrackRadius");
	ElectricityRadiusTimeline.SetPlayRate(1 / FirstElectricityRadiusDuration);
	ElectricityRadiusTimeline.PlayFromStart();

	ElectricityMovementTimeline.PlayFromStart();
}

#pragma region ElectricityRadius

void UENTElectricityComponent::ElectricityRadiusUpdate(float Alpha)
{
	if (!ElectricityFeedback)
	{
		return;
	}

	float Radius = FMath::Lerp(StartRadiusTarget, EndRadiusTarget, Alpha);
	ElectricityFeedback->SetRadius(Radius);
}

void UENTElectricityComponent::ElectricityRadiusFinished()
{
	if (EndRadiusTarget != SecondRadiusTarget)
	{
		return;
	}

	OnElectricityRadiusFinished.Broadcast();
}

#pragma endregion

#pragma region ElectricityMovement

void UENTElectricityComponent::ElectricityMovementUpdate(float Alpha)
{
	if (!ElectricityFeedback)
	{
		return;
	}

	OnElectricityMovementUpdated.Broadcast(Alpha);
}

void UENTElectricityComponent::ElectricityMovementFinished()
{
	OnElectricityMovementFinished.Broadcast();

	StartRadiusTarget = FirstRadiusTarget;
	EndRadiusTarget = SecondRadiusTarget;
	ElectricityRadiusTimeline.SetFloatCurve(SecondElectricityRadiusCurve, "TrackRadius");
	ElectricityRadiusTimeline.SetPlayRate(1 / SecondElectricityRadiusDuration);
	ElectricityRadiusTimeline.PlayFromStart();

	ElectricityOpacityTimeline.PlayFromStart();
}

#pragma endregion

#pragma region ElectricityOpacity
void UENTElectricityComponent::ElectricityOpacityUpdate(float Alpha)
{
	if (!ElectricityFeedback)
	{
		return;
	}

	float ScalarParam = FMath::Lerp(MaxOpacity, MinOpacity, Alpha);
	ElectricityFeedback->GetMaterial()->SetScalarParameterValue(ElectricityOpacityParam, ScalarParam);
}

void UENTElectricityComponent::ElectricityOpacityFinished()
{
	if (!ElectricityFeedback)
	{
		return;
	}

	ElectricityFeedback->Destroy();
	ElectricityFeedback = nullptr;

	OnElectricityOpacityFinished.Broadcast();
}
#pragma endregion
