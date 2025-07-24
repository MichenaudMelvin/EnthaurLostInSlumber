#include "UI/LevelEnteringUI.h"

#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LevelNameSubsystem.h"

void ULevelEnteringUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ULevelNameSubsystem* Subsystem = UGameplayStatics::GetGameInstance(this)->GetSubsystem<ULevelNameSubsystem>();
	if (!IsValid(Subsystem))
	{
		return;
	}

	AreaName->SetText(Subsystem->GetLevelName().AreaZone);
	RegionName->SetText(Subsystem->GetLevelName().RegionZone);
}
