// Fill out your copyright notice in the Description page of Project Settings.


#include "Navigation/ENTExcludeJumpNavAreaFilter.h"

#include "Navigation/ENTJumpNavArea.h"

UENTExcludeJumpNavAreaFilter::UENTExcludeJumpNavAreaFilter()
{
	FNavigationFilterArea FilterArea;
	FilterArea.AreaClass = UENTJumpNavArea::StaticClass();
	FilterArea.bIsExcluded = true;

	Areas.Add(FilterArea);
}
