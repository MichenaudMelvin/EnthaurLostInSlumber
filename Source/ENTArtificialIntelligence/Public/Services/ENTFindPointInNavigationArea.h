// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "ENTFindPointInNavigationArea.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTFindPointInNavigationArea : public UBTService
{
	GENERATED_BODY()

public:
	UENTFindPointInNavigationArea();

private:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

#if WITH_EDITOR
	virtual FString GetStaticDescription() const override;
#endif

	UPROPERTY(EditInstanceOnly, Category = "NavigationArea")
	FBlackboardKeySelector NavigationArea;

	UPROPERTY(EditInstanceOnly, Category = "NavigationArea", meta = (DisplayName = "RandomLocation"))
	FBlackboardKeySelector RandomLocationKey;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditInstanceOnly, Category = "Debug")
	bool bDebugService = false;

	UPROPERTY(EditInstanceOnly, Category = "Debug")
	FLinearColor DebugPointColor = FLinearColor::Blue;
#endif
};
