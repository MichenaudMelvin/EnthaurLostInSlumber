#include "HUD/ENTLevelEntering.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/ENTLevelNameSubsystem.h"

void UENTLevelEntering::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UENTLevelNameSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<UENTLevelNameSubsystem>();
	if (!IsValid(Subsystem))
	{
		return;
	}

	AreaName->SetText(Subsystem->GetLevelName().AreaZone);
	RegionName->SetText(Subsystem->GetLevelName().RegionZone);
}
