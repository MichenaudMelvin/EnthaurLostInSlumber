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

	RootPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootPivot);

	ParasiteCollision = CreateDefaultSubobject<UBoxComponent>("ParasiteHitBox");
	ParasiteCollision->SetupAttachment(RootPivot);

	ParasiteCollision->SetCollisionObjectType(ECC_Pawn);
	ParasiteCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ParasiteCollision->SetCollisionResponseToAllChannels(ECR_Block);

	ParasiteMesh = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh");
	ParasiteMesh->SetupAttachment(ParasiteCollision);

	MovementComponent = CreateDefaultSubobject<UGravityPawnMovement>("Movement");
	MovementComponent->MaxSpeed = 400.0f;
	MovementComponent->UpdatedComponent = ParasiteCollision;

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
	}
}

void AParasitePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!ParasiteCollision)
	{
		return;
	}

	ParasiteCollision->SetRelativeLocation(FVector(0, 0, ParasiteCollision->GetUnscaledBoxExtent().Z));

	if (!TargetPath)
	{
		return;
	}

	FHitResult HitResult;
	bool bHit = TargetPath->GetTracedPointLocation(0, HitResult);

	if (!bHit)
	{
		return;
	}

	SetActorLocation(HitResult.Location);

	FRotator Rotation = UKismetMathLibrary::MakeRotationFromAxes(FVector::ZeroVector, FVector::RightVector, HitResult.ImpactNormal);
	SetActorRotation(Rotation);


}

#if WITH_EDITOR
void AParasitePawn::PreEditChange(FProperty* PropertyAboutToChange)
{
	Super::PreEditChange(PropertyAboutToChange);

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
			bool bSuccess = TargetPath->AttachAI(this);

			if (!bSuccess)
			{
				TargetPath = nullptr;
				return;
			}
		}
	}
}
#endif
