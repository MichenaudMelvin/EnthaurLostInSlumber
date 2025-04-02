// Fill out your copyright notice in the Description page of Project Settings.


#include "Kevin/NerveReceptacle.h"

#include "Enum.h"
#include "Components/InteractableComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "Interface/NerveReactive.h"
#include "Kevin/Nerve.h"
#include "Kevin/PlayerToNervePhysicConstraint.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ANerveReceptacle::ANerveReceptacle()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NerveReceptacle = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Torus"));
	
	Collision = CreateDefaultSubobject<USphereComponent>("Collision");
	Collision->SetupAttachment(NerveReceptacle);
}

// Called when the game starts or when spawned
void ANerveReceptacle::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ANerveReceptacle::TriggerEnter);
}

// Called every frame
void ANerveReceptacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANerveReceptacle::TriggerEnter(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ANerve::StaticClass()))
	{
		ANerve* Nerve = Cast<ANerve>(OtherActor);
		Nerve->SetCurrentReceptacle(this);

		FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
		Nerve->GetComponentByClass<UStaticMeshComponent>()->AttachToComponent(GetRootComponent(), Rules);
		UGameplayStatics::GetPlayerCharacter(this, 0)->GetComponentByClass<UPlayerToNervePhysicConstraint>()->ReleasePlayer();

		Nerve->GetInteractable()->AddInteractable(Nerve->NerveBall);

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

