// Fill out your copyright notice in the Description page of Project Settings.


#include "ScriptedElements/ENTBehaviorTrigger.h"

#include "ENTDefaultAIController.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Saves/WorldSaves/ENTWorldSave.h"


AENTBehaviorTrigger::AENTBehaviorTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);
	RootComp->SetMobility(EComponentMobility::Static);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(RootComp);
	TriggerBox->SetMobility(EComponentMobility::Static);
}

void AENTBehaviorTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AENTBehaviorTrigger::OnEnterTriggerBox);

	if (!AIToTrigger)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("%s has no AIToTrigger"), *this->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
		FMessageLog("BlueprintLog").Error(FText::FromString(Message));
#endif
		return;
	}

	if (!AIToTrigger->GetController())
	{
		return;
	}

	AIController = Cast<AENTDefaultAIController>(AIToTrigger->GetController());
}

void AENTBehaviorTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	TriggerBox->OnComponentBeginOverlap.RemoveDynamic(this, &AENTBehaviorTrigger::OnEnterTriggerBox);
}

void AENTBehaviorTrigger::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	FVector BoxRelativeLocation = TriggerBox->GetRelativeLocation();
	BoxRelativeLocation.Z = BoxSize.Z;
	TriggerBox->SetRelativeLocation(BoxRelativeLocation);
	TriggerBox->SetBoxExtent(BoxSize);
}

#if WITH_EDITOR
void AENTBehaviorTrigger::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AENTBehaviorTrigger, AIToTrigger))
	{
		if (AIToTrigger)
		{
			if (!AIToTrigger->AIControllerClass->IsChildOf(AENTDefaultAIController::StaticClass()))
			{
				const FString Message = FString::Printf(TEXT("AIToTrigger must have a controller child of %s"), *AENTDefaultAIController::StaticClass()->GetName());

				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, Message);
				FMessageLog("BlueprintLog").Error(FText::FromString(Message));
			}
		}
	}
}
#endif

void AENTBehaviorTrigger::OnEnterTriggerBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AIController)
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	UAIPerceptionStimuliSourceComponent* PerceptionStimuliSource = OtherActor->GetComponentByClass<UAIPerceptionStimuliSourceComponent>();
	if (!PerceptionStimuliSource)
	{
		return;
	}

	AIController->RunCurrentBehaviorTree();

	if (bDestroyWhenTrigger)
	{
		Destroy();
	}
}

FENTGameElementData& AENTBehaviorTrigger::SaveGameElement(UENTWorldSave* CurrentWorldSave)
{
	Super::SaveGameElement(CurrentWorldSave);

	FETNScriptedAIElementData EmptyAIElementData;
	return CurrentWorldSave->ScriptedAIElementsData.Add(SaveID, EmptyAIElementData);
}

void AENTBehaviorTrigger::LoadGameElement(const FENTGameElementData& GameElementData, UENTWorldSave* LoadedWorldSave)
{
	Super::LoadGameElement(GameElementData, LoadedWorldSave);
}

void AENTBehaviorTrigger::FinishLoading(UENTWorldSave* LoadedWorldSave)
{
	Super::FinishLoading(LoadedWorldSave);
}
