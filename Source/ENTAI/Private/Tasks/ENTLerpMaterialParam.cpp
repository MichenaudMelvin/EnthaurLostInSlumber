// Fill out your copyright notice in the Description page of Project Settings.


#include "Tasks/ENTLerpMaterialParam.h"
#include "AIController.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UENTLerpMaterialParam::UENTLerpMaterialParam()
{
	bNotifyTick = true;
}

void UENTLerpMaterialParam::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
}

EBTNodeResult::Type UENTLerpMaterialParam::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	const UBlackboardComponent* CurrentBlackboard = OwnerComp.GetBlackboardComponent();
	if (!CurrentBlackboard)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	APawn* CurrentPawn = Controller->GetPawn();
	if (!CurrentPawn)
	{
		return EBTNodeResult::Failed;
	}

	TArray<UMeshComponent*> MeshComponents;
	CurrentPawn->GetComponents<UMeshComponent>(MeshComponents, false);

	Materials.Empty();
	for (UMeshComponent* MeshComponent : MeshComponents)
	{
		if (MaterialIndex != -1)
		{
			AddDynamicMaterial(MeshComponent, MaterialIndex);
			continue;
		}

		for (int i = 0; i < MeshComponent->GetMaterials().Num(); i++)
		{
			AddDynamicMaterial(MeshComponent, i);
		}
	}

	return EBTNodeResult::InProgress;
}

void UENTLerpMaterialParam::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!Materials.IsValidIndex(0) || !Materials[0])
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	FMaterialParameterInfo ParameterInfo(ParamName);

	float Alpha = DeltaSeconds * LerpSpeed;

	bool bCanFinish = true;
	for (TObjectPtr<UMaterialInstanceDynamic> Material : Materials)
	{
		if (bUseVectorParam)
		{
			FLinearColor CurrentColor;
			bool bFind = Material->GetVectorParameterValue(ParameterInfo, CurrentColor);
			if (!bFind)
			{
				continue;
			}

			FLinearColor ColorResult = UKismetMathLibrary::LinearColorLerp(CurrentColor, TargetColor, Alpha);
			Material->SetVectorParameterValue(ParamName, ColorResult);

			if (!CurrentColor.Equals(ColorResult, EqualTolerance))
			{
				bCanFinish = false;
			}
		}
		else
		{
			float CurrentFloat;
			bool bFind = Material->GetScalarParameterValue(ParameterInfo, CurrentFloat);
			if (!bFind)
			{
				continue;
			}

			float FloatResult = FMath::Lerp(CurrentFloat, TargetFloat, Alpha);
			Material->SetScalarParameterValue(ParamName, FloatResult);

			if (!FMath::IsNearlyEqual(CurrentFloat, TargetFloat, EqualTolerance))
			{
				bCanFinish = false;
			}
		}
	}

	if (!bCanFinish)
	{
		return;
	}

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

void UENTLerpMaterialParam::AddDynamicMaterial(UMeshComponent* MeshComponent, int32 InMaterialIndex)
{
	UMaterialInterface* Material = MeshComponent->GetMaterial(InMaterialIndex);
	if (!Material)
	{
		return;
	}

	UMaterialInstanceDynamic* DynamicMaterial = Cast<UMaterialInstanceDynamic>(Material);
	if (!DynamicMaterial)
	{
		DynamicMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, Material);
		MeshComponent->SetMaterial(InMaterialIndex, DynamicMaterial);
	}

	Materials.Add(DynamicMaterial);
}
