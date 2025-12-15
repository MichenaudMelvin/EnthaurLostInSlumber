// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "ENTExcludeJumpNavAreaFilter.generated.h"

UCLASS()
class ENTARTIFICIALINTELLIGENCE_API UENTExcludeJumpNavAreaFilter : public UNavigationQueryFilter
{
	GENERATED_BODY()

public:
	UENTExcludeJumpNavAreaFilter();
};
