// Fill out your copyright notice in the Description page of Project Settings.


#include "Config/ENTCoreConfig.h"

UENTCoreConfig::UENTCoreConfig()
{
	GroundObjectTypes.Add(ObjectTypeQuery1);
	GroundObjectTypes.Add(ObjectTypeQuery2);
}

#if WITH_EDITOR

void UENTCoreConfig::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName& ChangedProperty = PropertyChangedEvent.GetMemberPropertyName();

	if (ChangedProperty == GET_MEMBER_NAME_CHECKED(UENTCoreConfig, CameraPropulsion))
	{
		CameraPropulsion.SetLowerBoundValue(FMath::Clamp(CameraPropulsion.GetLowerBoundValue(), -1.0f, 1.0f));
		CameraPropulsion.SetUpperBoundValue(FMath::Clamp(CameraPropulsion.GetUpperBoundValue(), -1.0f, 1.0f));
	}
}

#endif

UAkSwitchValue* UENTCoreConfig::FindNoise(const EPhysicalSurface& Surface) const
{
	const TSoftObjectPtr<UAkSwitchValue>* SoftObjectPtr = SurfaceNoises.Find(Surface);
	if (!SoftObjectPtr)
	{
		return nullptr;
	}

	const TSoftObjectPtr<UAkSwitchValue> SoftObject = *SoftObjectPtr;
	return SoftObject.LoadSynchronous();
}
