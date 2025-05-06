// Fill out your copyright notice in the Description page of Project Settings.


#include "Physics/SurfaceSettings.h"

UAkSwitchValue* USurfaceSettings::FindNoise(const EPhysicalSurface& Surface) const
{
	const TSoftObjectPtr<UAkSwitchValue>* SoftObjectPtr = SurfaceNoises.Find(Surface);
	if (!SoftObjectPtr)
	{
		return nullptr;
	}

	const TSoftObjectPtr<UAkSwitchValue> SoftObject = *SoftObjectPtr;
	return SoftObject.LoadSynchronous();
}
