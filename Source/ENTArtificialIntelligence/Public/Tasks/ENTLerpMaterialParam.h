// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ENTLerpMaterialParam.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTLerpMaterialParam : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UENTLerpMaterialParam();

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, Category = "Material")
	FName ParamName = NAME_None;

	UPROPERTY()
	bool bUseVectorParam = false;

	UPROPERTY(EditInstanceOnly, Category = "Material", meta = (EditCondition = bUseVectorParam))
	FLinearColor TargetColor;

	UPROPERTY(EditInstanceOnly, Category = "Material", meta = (EditCondition = "!bUseVectorParam"))
	float TargetFloat;

	UPROPERTY(EditInstanceOnly, Category = "Material")
	float LerpSpeed = 1.0f;

	UPROPERTY(EditInstanceOnly, Category = "Material")
	float EqualTolerance = 0.0001f;

	/**
	 * @brief -1 will take all materials
	 */
	UPROPERTY(EditInstanceOnly, Category = "Material", meta = (ClampMin = -1))
	int MaterialIndex = -1;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> Materials;

	void AddDynamicMaterial(UMeshComponent* MeshComponent, int32 InMaterialIndex);
};
