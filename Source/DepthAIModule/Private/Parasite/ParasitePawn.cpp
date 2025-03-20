// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Parasite/ParasiteController.h"
#include "Path/AIPath.h"

AParasitePawn::AParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false;

	Box = CreateDefaultSubobject<UBoxComponent>("ParasiteHitBox");
	SetRootComponent(Box);

	Box->SetCollisionObjectType(ECC_Pawn);
	Box->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Box->SetCollisionResponseToAllChannels(ECR_Block);

	ParasiteMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	ParasiteMesh->SetupAttachment(RootComponent);

	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>("Movement");
	MovementComponent->MaxSpeed = 400.0f;

	AIControllerClass = AParasiteController::StaticClass();
}

void AParasitePawn::BeginPlay()
{
	Super::BeginPlay();

	AParasiteController* ParasiteController = Cast<AParasiteController>(Controller);

	if (!ParasiteController)
	{
#if WITH_EDITOR
		const FString Message = FString::Printf(TEXT("%s has wrong AIController, must be %s"), *GetClass()->GetName(), *AParasiteController::StaticClass()->GetName());

		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
		FMessageLog("BlueprintLog").Warning(FText::FromString(Message));
#endif
		return;
	}

	if (TargetPath && ParasiteController->GetBlackboardComponent())
	{
		ParasiteController->GetBlackboardComponent()->SetValueAsObject(PathKeyName, TargetPath);
	}
}

