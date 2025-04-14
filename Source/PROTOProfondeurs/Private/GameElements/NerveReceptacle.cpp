// Fill out your copyright notice in the Description page of Project Settings.


#include "GameElements/NerveReceptacle.h"

#include "Enumerations.h"
#include "Components/InteractableComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/NerveReactive.h"
#include "GameElements/Nerve.h"
#include "Components/PlayerToNervePhysicConstraint.h"
#include "Kismet/GameplayStatics.h"


ANerveReceptacle::ANerveReceptacle()
{
	PrimaryActorTick.bCanEverTick = false;

	NerveReceptacle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Torus"));

	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(NerveReceptacle);
}

void ANerveReceptacle::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ANerveReceptacle::TriggerEnter);
}

void ANerveReceptacle::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ANerve::StaticClass()))
	{
		ANerve* Nerve = Cast<ANerve>(OtherActor);
		Nerve->SetCurrentReceptacle(this);

		OnNerveConnect();

		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		Nerve->GetComponentByClass<UStaticMeshComponent>()->AttachToComponent(GetRootComponent(), Rules);
		UGameplayStatics::GetPlayerCharacter(this, 0)->GetComponentByClass<UPlayerToNervePhysicConstraint>()->ReleasePlayer();

		Nerve->GetInteractable()->AddInteractable(Nerve->GetNerveBall());

		TriggerLinkedObjects();
	}
}

void ANerveReceptacle::TriggerLinkedObjects()
{
	TArray<AActor*> Actors;
	ObjectReactive.GetKeys(Actors);

	for (AActor* Actor : Actors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		if (Actor->Implements<UNerveReactive>())
		{
			if (ObjectReactive[Actor] == ENerveReactiveInteractionType::ForceDefaultState)
			{
				INerveReactive::Execute_SetLock(Actor, true);
			} else
			{
				INerveReactive::Execute_Trigger(Actor);
			}
		}
	}
}

