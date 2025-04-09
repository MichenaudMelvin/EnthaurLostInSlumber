// Fill out your copyright notice in the Description page of Project Settings.


#include "PROTOProfondeurs/Public/GameElements/RespawnTree.h"

#include "FCTween.h"
#include "Components/InteractableComponent.h"
#include "Components/LightComponent.h"
#include "Components/PointLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FirstPersonCharacter.h"


// Sets default values
ARespawnTree::ARespawnTree()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	TreeModel = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TreeModel"));
	TreeModel->SetupAttachment(Root);

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Respawn Point"));
	RespawnPoint->SetupAttachment(Root);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(Root);

	Interaction = CreateDefaultSubobject<UInteractableComponent>(TEXT("Interaction"));
	Interaction->OnInteract.AddDynamic(this, &ARespawnTree::ActivateRespawn);
}

// Called when the game starts or when spawned
void ARespawnTree::BeginPlay()
{
	Super::BeginPlay();

	BulbMaterial = TreeModel->CreateDynamicMaterialInstance(2, TreeModel->GetMaterial(2));
	TreeModel->SetMaterial(2, BulbMaterial);
	BulbMaterial->SetScalarParameterValue("State", 2.f);

	Interaction->AddInteractable(TreeModel);
}

void ARespawnTree::ActivateRespawn(APlayerController* PlayerController, APawn* Pawn)
{
	if (bIsActivated)
		return;
	
	Cast<AFirstPersonCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0))->SetRespawnPosition(RespawnPoint->GetComponentLocation());
	bIsActivated = true;
	Interaction->RemoveInteractable(TreeModel);

	FCTween::Play(
			2.f,
			0.f,
			[&](float x)
			{
				BulbMaterial->SetScalarParameterValue("State", x);
			},
			0.5f,
			EFCEase::InSine);

	FCTween::Play(
		0.f,
		lightLevel,
		[&](float intensity)
		{
			Light->SetIntensity(intensity);
		},
		0.5f,
		EFCEase::InSine
	);
}



