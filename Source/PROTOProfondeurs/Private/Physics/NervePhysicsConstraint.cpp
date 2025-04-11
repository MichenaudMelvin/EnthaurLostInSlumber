// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/NervePhysicsConstraint.h"

#if WITH_EDITOR

void UNervePhysicsConstraint::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(UNervePhysicsConstraint, CameraPropulsion))
	{
		CameraPropulsion.SetLowerBoundValue(FMath::Clamp(CameraPropulsion.GetLowerBoundValue(), -1.0f, 1.0f));
		CameraPropulsion.SetUpperBoundValue(FMath::Clamp(CameraPropulsion.GetUpperBoundValue(), -1.0f, 1.0f));
	}
}

#endif