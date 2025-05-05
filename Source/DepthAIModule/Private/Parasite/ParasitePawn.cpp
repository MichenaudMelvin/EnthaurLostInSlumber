// Fill out your copyright notice in the Description page of Project Settings.


#include "Parasite/ParasitePawn.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Components/GravityPawnMovement.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "Parasite/ParasiteController.h"
#include "Path/AIPath.h"

AParasitePawn::AParasitePawn()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ParasiteCollision = CreateDefaultSubobject<UBoxComponent>("ParasiteHitBox");
	SetRootComponent(ParasiteCollision);

	ParasiteCollision->SetCollisionObjectType(ECC_Pawn);
	ParasiteCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ParasiteCollision->SetCollisionResponseToAllChannels(ECR_Block);

	ParasiteMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	ParasiteMesh->SetupAttachment(ParasiteCollision);

	MovementComponent = CreateDefaultSubobject<UGravityPawnMovement>("Movement");
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
		ParasiteController->GetBlackboardComponent()->SetValueAsBool(WalkOnFloorKeyName, TargetPath->IsOnFloor());

		if (!TargetPath->IsOnFloor())
		{
			MovementComponent->SetGravityScale(0.0f);
		}
	}
}

void AParasitePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!ParasiteCollision)
	{
		return;
	}

	if (!TargetPath)
	{
		return;
	}

#if WITH_EDITORONLY_DATA
	bool bIsAttached = TargetPath->AttachAI(this);

	if (!bIsAttached)
	{
		return;
	}
#endif

	FHitResult HitResult;
	bool bHit = TargetPath->GetTracedPointLocation(0, HitResult);

	if (!bHit)
	{
		return;
	}

	FVector ActorLocation = HitResult.Location;
	ActorLocation += (TargetPath->GetDirection() * -1 * ParasiteCollision->GetUnscaledBoxExtent().Z);
	SetActorLocation(ActorLocation);

	FRotator Rotation = UKismetMathLibrary::MakeRotFromZ(HitResult.Normal);
	SetActorRotation(Rotation);
}

#if WITH_EDITOR
void AParasitePawn::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

	if (!PropertyAboutToChange)
	{
		return;
	}

	if (PropertyAboutToChange->NamePrivate == GET_MEMBER_NAME_CHECKED(AParasitePawn, TargetPath))
	{
		if (TargetPath)
		{
			TargetPath->DetachAI(this);
		}
	}
}

void AParasitePawn::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(AParasitePawn, TargetPath))
	{
		if (TargetPath)
		{
			bool bIsAttached = TargetPath->AttachAI(this);

			if (!bIsAttached)
			{
				TargetPath = nullptr;
			}
		}
	}
}
#endif
